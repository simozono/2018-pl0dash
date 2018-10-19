# LR 構文解析プログラム
#
class LRTable
  # action 表をクラス変数として定義
  @@action_table     = Array.new
  @@action_table[0]  = { 'n' => [:shift, 5], '(' => [:shift, 4] }
  @@action_table[1]  = { '+' => [:shift, 6], '$' => [:acc, 0] }
  @@action_table[2]  = { '+' => [:reduce, 2], '*' => [:shift, 7],
                         ')' => [:reduce, 2], '$' => [:reduce, 2] }
  @@action_table[3]  = { '+' => [:reduce, 4], '*' => [:reduce, 4],
                         ')' => [:reduce, 4], '$' => [:reduce, 4] }
  @@action_table[4]  = { 'n' => [:shift, 5], '(' => [:shift, 4] }
  @@action_table[5]  = { '+' => [:reduce, 6], '*' => [:reduce, 6],
                         ')' => [:reduce, 6], '$' => [:reduce, 6] }
  @@action_table[6]  = { 'n' => [:shift, 5], '(' => [:shift, 4] }
  @@action_table[7]  = { 'n' => [:shift, 5], '(' => [:shift, 4] }
  @@action_table[8]  = { '+' => [:shift, 6], ')' => [:shift, 11] }
  @@action_table[9]  = { '+' => [:reduce, 1], '*' => [:shift, 7],
                         ')' => [:reduce, 1], '$' => [:reduce, 1] }
  @@action_table[10] = { '+' => [:reduce, 3], '*' => [:reduce, 3],
                         ')' => [:reduce, 3], '$' => [:reduce, 3] }
  @@action_table[11] = { '+' => [:reduce, 5], '*' => [:reduce, 5],
                         ')' => [:reduce, 5], '$' => [:reduce, 5] }

  # goto 表をクラス変数として定義
  @@goto_table    = Array.new
  @@goto_table[0] = { 'E' => 1, 'T' => 2, 'F' => 3 }
  @@goto_table[4] = { 'E' => 8, 'T' => 2, 'F' => 3 }
  @@goto_table[6] = { 'T' => 9, 'F' => 3 }
  @@goto_table[7] = { 'F' => 10 }

  # 生成規則をクラス変数として定義
  @@rule_table    = Array.new
  @@rule_table[1] = { 'E' => 'E+T' }
  @@rule_table[2] = { 'E' => 'T' }
  @@rule_table[3] = { 'T' => 'T*F' }
  @@rule_table[4] = { 'T' => 'F' }
  @@rule_table[5] = { 'F' => '(E)' }
  @@rule_table[6] = { 'F' => 'n' }

  def initialize
  end

  def action(terminal_symbol, state)
    symbol = terminal_symbol =~ /^\d+$/ ? 'n' : terminal_symbol
    ret    = @@action_table[state][symbol]
    if ret == nil
      puts '構文エラー:action表に存在しない'
      exit
    end
    ret
  end

  def goto(non_terminal_symbol, state)
    @@goto_table[state][non_terminal_symbol]
  end

  def rule(number)
    @@rule_table[number].to_a.first
  end
end

def result_print(line, stack, input) # きれいに表示させるためのメソッド
  if stack.kind_of?(String)
    puts "[#{line.to_s.rjust(2)}] #{stack}"
  else
    puts "[#{line.to_s.rjust(2)}] #{stack.join(' ').ljust(30)} #{input.join.rjust(20)}"
  end
end

# 解析する元の文字列(入力行)
print '文法G5で受理できる式を入力してください: '
input = gets.chomp + '$'
# input = "n*(n+n)$"

lr    = LRTable.new #
stack = Array.new # スタック
count = 1 # 処理の行番号

# 最初の処理
input_array = input.chars.to_a # 入力を一文字ずつ分割
stack.push 0
result_print(count, stack, input_array)

# 以後の処理
loop do # 無限ループ
  count += 1
  (result_act, result_state) = lr.action(input_array.first, stack.last)

  case result_act
  when :acc # acc に到達したら終了
    result_print(count, 'accになったので終了', nil)
    break
  when :shift # shift 動作
    stack.push input_array.shift
    stack.push result_state
  when :reduce # reduce 動作
    (left, right) = lr.rule(result_state)
    (right.size * 2).times { stack.pop } # 不真面目なreduce処理
    top_state = stack.last
    stack.push left
    stack.push lr.goto(left, top_state)
  else
    # type code here
  end
  result_print(count, stack, input_array)
end
