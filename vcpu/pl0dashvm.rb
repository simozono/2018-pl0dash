#! /usr/bin/env ruby

require 'optparse'

class Pl0DashVM
  MAX_MEM    = 1000
  HEAP_START = 800

  def initialize(lines)
    init_reg_mem # レジスタとメモリ初期化
    lines.each.with_index(1) do |line, i|
      @memory[i] = line.split(/\s+|\s*,\s*/)
    end
  end

  def list_code_area
    @memory[1..(HEAP_START - 1)].each.with_index(1) do |item, i|
      break if item.nil?

      printf "%4d %s\n", i, [item[0], item[1..2].join(',')].join(' ').strip
    end
  end

  def execute(show: false)
    @reg[:pc] = 1
    loop do
      item = @memory[@reg[:pc]]
      if show
        printf "%4d %s\n", @reg[:pc],
               [item[0], item[1..2].join(',')].join(' ').strip
      end
      exec_status = execute_line(@memory[@reg[:pc]])
      show_vcpu_status if show
      break unless exec_status
    end
  end

  def execute_line(line)
    (op, opr1, opr2) = line
    case op.upcase
    when 'LOAD'
      move(ref_op(opr2), opr1)
      @reg[:pc] += 1
    when 'STORE', 'MOVE'
      move(ref_op(opr1), opr2)
      @reg[:pc] += 1
    when 'PUSH'
      @reg[:sp] -= 1
      move(ref_op(opr1), '#(SP)')
      @reg[:pc] += 1
    when 'POP'
      move(@memory[@reg[:sp]], opr1)
      @reg[:sp] += 1
      @reg[:pc] += 1
    when 'JMP'
      @reg[:pc] = ref_op(opr1)
    when 'JPC'
      if (@reg[:c]).zero?
        @reg[:pc] = ref_op(opr1)
      else
        @reg[:pc] += 1
      end
    when 'PUSHUP'
      @reg[:sp] -= 1
      @reg[:pc] += 1
    when 'CALL'
      @reg[:sp] -= 1
      @memory[@reg[:sp]] = @reg[:pc] + 1
      @reg[:pc]          = ref_op(opr1)
    when 'RET'
      @reg[:pc] = @memory[@reg[:sp]]
      @reg[:sp] += 1
      @reg[:sp] += ref_op(opr1)
    when 'PRINT'
      print ref_op(opr1)
      @reg[:pc] += 1
    when 'PRINTLN'
      puts
      @reg[:pc] += 1
    when 'END'
      @reg[:pc] = -1
      return false
      # 以下演算子
    when 'PLUS'
      @reg[:c] = @reg[:a] + @reg[:b]
      @reg[:pc] += 1
    when 'MINUS'
      @reg[:c] = @reg[:a] - @reg[:b]
      @reg[:pc] += 1
    when 'MULTI'
      @reg[:c] = @reg[:a] * @reg[:b]
      @reg[:pc] += 1
    when 'DIV'
      @reg[:c] = (@reg[:a] / @reg[:b]).to_i
      @reg[:pc] += 1
    when 'CMPODD'
      @reg[:c] = @reg[:a].odd? ? 1 : 0
      @reg[:pc] += 1
    when 'CMPEQ'
      @reg[:c] = @reg[:a].to_i == @reg[:b].to_i ? 1 : 0
      @reg[:pc] += 1
    when 'CMPNOTEQ'
      @reg[:c] = @reg[:a].to_i != @reg[:b].to_i ? 1 : 0
      @reg[:pc] += 1
    when 'CMPLT'
      @reg[:c] = @reg[:a].to_i < @reg[:b].to_i ? 1 : 0
      @reg[:pc] += 1
    when 'CMPGT'
      @reg[:c] = @reg[:a].to_i > @reg[:b].to_i ? 1 : 0
      @reg[:pc] += 1
    when 'CMPLE'
      @reg[:c] = @reg[:a].to_i <= @reg[:b].to_i ? 1 : 0
      @reg[:pc] += 1
    when 'CMPGE'
      @reg[:c] = @reg[:a].to_i >= @reg[:b].to_i ? 1 : 0
      @reg[:pc] += 1
    else
      abort 'ASM CODE ERROR'
    end
  end

  private

  def move(value, sto_op) # valueをsto_opに代入
    case sto_op.downcase
    when /^([a-z]+)$/ # レジスタのはず
      @reg[Regexp.last_match(1).to_sym] = value
    when /^#\((\d+)\)$/ # #(数字)のはず
      @memory[Regexp.last_match(1).to_i] = value
    when /^#\(([a-z]+)\)$/ # #(レジスタ)のはず
      @memory[@reg[Regexp.last_match(1).to_sym]] = value
    when /^#\(fp([+|\-]\d+)\)$/ # #(FP(+|-)数字)のはず
      @memory[@reg[:fp] + Regexp.last_match(1).to_i] = value
    else
      abort 'ASM OPERAND2 ERROR'
    end
  end

  def ref_op(opr)
    case opr.downcase
    when /^(\d+)$/ # 数字
      Regexp.last_match(1).to_i
    when /^([a-z]+)$/ # レジスタのはず
      @reg[Regexp.last_match(1).to_sym]
    when /^#\((\d+)\)$/ # #(数字)のはず
      @memory[Regexp.last_match(1).to_i]
    when /^#\(([a-z]+)\)$/ # #(レジスタ)のはず
      @memory[@reg[Regexp.last_match(1).to_sym]]
    when /^#\(fp([+|\-]\d+)\)$/ # #(FP(+|-)数字)のはず
      @memory[@reg[:fp] + Regexp.last_match(1).to_i]
    else
      abort 'ASM OPERAND1 ERROR'
    end
  end

  def init_reg_mem
    @memory    = Array.new(MAX_MEM + 2)
    @memory[0] = 'FirstLine'
    @reg       = {}
    @reg[:a]   = 0
    @reg[:b]   = 0
    @reg[:c]   = 0
    @reg[:sp]  = MAX_MEM + 1
    @reg[:fp]  = MAX_MEM + 1
    @reg[:pc]  = 0
  end

  def show_vcpu_status
    show_reg
    show_heap
    show_stack
  end

  def show_reg
    msg = "============\n"
    msg += "    Register\n"
    msg += '    '
    msg += @reg.map do |reg, val|
      format('%3s:%4d', reg, val)
    end.join(' ')
    puts msg
  end

  def show_heap
    i   = HEAP_START - 1
    msg = "    Heap Area\n"
    @memory[HEAP_START...(HEAP_START + 10)].each_slice(5) do |row|
      msg += '   '
      msg += row.map do |val|
        i += 1
        format(' %4d:%4s', i, val)
      end.join(' ')
      msg += "\n"
    end
    puts msg
  end

  def show_stack
    i   = ((@reg[:sp].to_f - 2) / 10).floor.*(10).to_i
    msg = "    Stack Area\n"
    @memory[(i + 1)...-1].each_slice(5) do |row|
      msg += '   '
      msg += row.map do |val|
        i += 1
        format(' %4d:%4s', i, (i < @reg[:sp] ? nil : val))
      end.join(' ')
      msg += "\n"
    end
    msg += "============\n"
    puts msg
  end
end

# アセンブリ言語ソースコードの読込
#   引数があったらそれをファイル名としオープンする
#   引数がなかったら標準入力から読み込む
#

opt     = OptionParser.new
options = {}

opt.on('-d') { |i| options[:show] = i }

opt.parse!(ARGV)

if ARGV[0].nil?
  lines = $stdin.readlines
else
  begin
    file  = File.open(ARGV[0])
    lines = file.readlines
    file.close
  rescue Errno::ENOENT, Errno::EACCES => err
    abort err.message
  end
end

vm = Pl0DashVM.new(lines)
# vm.list_code_area
vm.execute options
