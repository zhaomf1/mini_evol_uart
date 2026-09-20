from docx import Document
from docx.shared import Pt, Cm
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT, WD_CELL_VERTICAL_ALIGNMENT
from docx.oxml import OxmlElement
from docx.oxml.ns import qn

OUT = 'docs/PWM_UART_binary_protocol_V1.docx'

def shade(cell, fill):
    tcPr = cell._tc.get_or_add_tcPr()
    shd = OxmlElement('w:shd')
    shd.set(qn('w:fill'), fill)
    tcPr.append(shd)

def set_cell(cell, text, bold=False):
    cell.text = str(text)
    for p in cell.paragraphs:
        for r in p.runs:
            r.font.name = 'Microsoft YaHei'
            r.font.size = Pt(9)
            r.bold = bold
    cell.vertical_alignment = WD_CELL_VERTICAL_ALIGNMENT.CENTER

def style_table(table):
    table.alignment = WD_TABLE_ALIGNMENT.CENTER
    table.style = 'Table Grid'

doc = Document()
sec = doc.sections[0]
sec.top_margin = Cm(1.8); sec.bottom_margin = Cm(1.8)
sec.left_margin = Cm(1.8); sec.right_margin = Cm(1.8)
doc.styles['Normal'].font.name = 'Microsoft YaHei'
doc.styles['Normal'].font.size = Pt(10)

p = doc.add_paragraph(style='Title')
p.alignment = WD_ALIGN_PARAGRAPH.CENTER
p.add_run('PWM输出通信协议 V1.0')
p = doc.add_paragraph()
p.alignment = WD_ALIGN_PARAGRAPH.CENTER
p.add_run('适用于PH主控板 LED-R/PD13 PWM输出').font.size = Pt(10)

doc.add_heading('1 协议说明', level=1)
doc.add_paragraph('本协议用于通过UART1控制PWM输出，协议格式参考MISScell主控板通信协议。每帧固定14字节，设备收到完整帧后执行命令并返回同格式应答。')
doc.add_paragraph('串口参数：9600 baud，8数据位，1停止位，无校验。若系统配置为2000000 baud，仅改变波特率，数据帧格式不变。')

doc.add_heading('2 协议帧格式', level=1)
t = doc.add_table(rows=1, cols=4); style_table(t)
for c, v in zip(t.rows[0].cells, ['字节序号', '数据', '说明', '备注']):
    set_cell(c, v, True); shade(c, 'D9EAF7')
rows = [('00','0x55','帧头',''),('01','0xAA','帧头',''),('02','0x07','设备编号','PWM输出模块'),('03','CMD','命令码',''),('04～07','DATA0～DATA3','参数1','大端序'),('08～11','DATA4～DATA7','参数2','大端序'),('12','0x5A','帧尾',''),('13','0xA5','帧尾','')]
for row in rows:
    cells = t.add_row().cells
    for c, v in zip(cells, row): set_cell(c, v)
doc.add_paragraph('多字节整数采用大端序，高字节在前。接收端应校验帧头、设备编号、固定帧长和帧尾。')

doc.add_heading('3 命令定义', level=1)
t = doc.add_table(rows=1, cols=4); style_table(t)
for c, v in zip(t.rows[0].cells, ['命令码','命令名称','DATA0～DATA3','DATA4～DATA7']):
    set_cell(c, v, True); shade(c, 'D9EAF7')
for row in [('0x01','有限周期启动','频率Hz uint32','周期数 uint32'),('0x02','连续PWM启动','频率Hz uint32','固定填0'),('0x03','停止PWM','固定填0','固定填0'),('0xFF','查询版本','固定填0','固定填0')]:
    cells = t.add_row().cells
    for c, v in zip(cells, row): set_cell(c, v)

doc.add_heading('4 参数范围和执行规则', level=1)
for s in ['频率范围：1～100000 Hz。','有限周期启动：周期数范围1～1000000，完成后自动停止并保持LED-R/PD13为低电平。','连续PWM启动：周期数字段必须为0，持续输出直到收到停止命令。','PWM运行期间再次收到启动命令，返回忙错误，不截断当前输出。','收到完整命令帧至PWM硬件启动完成期间，LED-B/PD15保持高电平，完成后拉低。','有限周期完成不发送异步完成帧。']:
    doc.add_paragraph(s, style='List Bullet')

doc.add_heading('5 应答帧', level=1)
doc.add_paragraph('命令执行成功时，设备完整返回收到的原始14字节数据帧。上位机收到与发送帧完全一致的数据，即认为命令执行成功。')
doc.add_paragraph('命令执行失败时，设备仍返回14字节数据帧，但将字节03改为 0x80 | 原命令码，字节04返回错误码，其他参数字节保留原值。')
t = doc.add_table(rows=1, cols=2); style_table(t)
for c, v in zip(t.rows[0].cells, ['结果码','含义']):
    set_cell(c, v, True); shade(c, 'D9EAF7')
for row in [('0x00','执行成功'),('0x01','帧格式错误'),('0x02','频率超范围'),('0x03','周期数超范围'),('0x04','参数组合错误'),('0x05','PWM正在运行'),('0x06','不支持的命令')]:
    cells = t.add_row().cells
    for c, v in zip(cells, row): set_cell(c, v)
doc.add_paragraph('查询版本命令成功时，字节04～06依次返回版本主版本、次版本和修订版本，字节07～12填0。上位机将三个字节组合为 V主版本.次版本.修订版本，例如 0x01、0x01、0x01 显示为 V1.1.1。该命令成功响应属于版本查询特例，不按普通命令完整回显。')

doc.add_heading('6 数据帧示例', level=1)
examples = [('10kHz输出10000个周期','55 AA 07 01 00 00 27 10 00 00 27 10 5A A5'),('连续输出10kHz','55 AA 07 02 00 00 27 10 00 00 00 00 5A A5'),('停止PWM','55 AA 07 03 00 00 00 00 00 00 00 00 5A A5'),('查询版本请求','55 AA 07 FF 00 00 00 00 00 00 00 00 5A A5'),('查询版本响应 V1.1.1','55 AA 07 FF 01 01 01 00 00 00 00 00 5A A5')]
t = doc.add_table(rows=1, cols=2); style_table(t)
for c, v in zip(t.rows[0].cells, ['功能','发送数据']):
    set_cell(c, v, True); shade(c, 'D9EAF7')
for row in examples:
    cells = t.add_row().cells
    for c, v in zip(cells, row): set_cell(c, v)

doc.add_paragraph('版本：V1.0    日期：2026年9月18日')
doc.save(OUT)
print(OUT)
