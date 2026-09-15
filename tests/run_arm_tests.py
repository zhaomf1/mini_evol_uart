"""Run real ARMCC-compiled pure-C tests in Unicorn (no peripheral emulation).

Requires: pip install --target tests/.runtime unicorn pyelftools
"""
import io
import pathlib
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tests/.runtime'))
from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_HOOK_CODE
from unicorn.arm_const import UC_ARM_REG_SP, UC_ARM_REG_LR, UC_ARM_REG_R0

BIN = pathlib.Path('C:/Keil_v5/ARM/ARMCC/bin')
BUILD = ROOT / 'tests/.build'
BUILD.mkdir(exist_ok=True)


def run_test(name):
    objects = []
    for source in [ROOT / f'tests/test_{name}.c', ROOT / f'Core/Src/app/{name}.c']:
        obj = BUILD / (source.stem + '.o')
        subprocess.run([str(BIN / 'armcc.exe'), '--cpu=Cortex-M4', '--c99', '-g',
                        '-O0', '-I' + str(ROOT / 'Core/Inc/app'), '-c', str(source),
                        '-o', str(obj)], check=True)
        objects.append(str(obj))
    axf = BUILD / (name + '.axf')
    subprocess.run([str(BIN / 'armlink.exe'), '--entry=main', '--ro_base=0x10000',
                    '--rw_base=0x20000000', *objects, '-o', str(axf)], check=True)
    elf = ELFFile(io.BytesIO(axf.read_bytes()))
    uc = Uc(UC_ARCH_ARM, UC_MODE_THUMB)
    uc.mem_map(0x10000, 0x100000)
    uc.mem_map(0x20000000, 0x100000)
    for segment in elf.iter_segments():
        if segment['p_type'] == 'PT_LOAD':
            uc.mem_write(segment['p_vaddr'], segment.data())
    symbols = {s.name: s['st_value'] for s in elf.get_section_by_name('.symtab').iter_symbols()}
    failures = []
    fail_addresses = {v & ~1 for k, v in symbols.items() if k in ('__aeabi_assert', '__assert', 'abort')}

    def hook(machine, address, size, data):
        if address in fail_addresses:
            failures.append('C assertion failed')
            machine.emu_stop()

    uc.hook_add(UC_HOOK_CODE, hook)
    uc.reg_write(UC_ARM_REG_SP, 0x200FF000)
    uc.reg_write(UC_ARM_REG_LR, 0xF0001)
    uc.emu_start(symbols['main'] | 1, 0xF0000, count=1000000)
    if failures or uc.reg_read(UC_ARM_REG_R0) != 0:
        raise AssertionError(name + ': ' + str(failures))
    print('PASS', name)


if __name__ == '__main__':
    for name in sys.argv[1:] or ['pwm_command', 'pwm_timer_config', 'pwm_timing_marker']:
        run_test(name)
