const { join } = require('path')
const { readFileSync } = require('fs')

const input = readFileSync(join(__dirname, './input.txt'), 'utf-8')
const inputtest = `root: pppw + sjmn
dbpl: 5
cczh: sllz + lgvd
zczc: 2
ptdq: humn - dvpt
dvpt: 3
lfqf: 4
humn: 5
ljgn: 2
sjmn: drzm * dbpl
sllz: 4
pppw: cczh / lfqf
lgvd: ljgn * ptdq
drzm: hmdt - zczc
hmdt: 32`
const equations = input
  .split('\n')
  .filter(Boolean)
  .map((line) => line.replace(/:/, ' ='))

let rootVal = null
// console.log(equations)

while (!rootVal) {
  equations.forEach((equation) => {
    try {
      eval(equation)
    } catch (e) {
      // do fuck all
      // console.log(e)
    }
  })

  try {
    rootVal = eval('root')
  } catch (e) {
    // console.log('no root')
  }
}

console.log('Part one:', rootVal)
