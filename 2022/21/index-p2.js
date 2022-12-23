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
  .reduce((eqs, line) => {
    let [variable, eq] = line.split(': ')
    if (variable === 'root') {
      eq = eq.replace(/\+/, '==')
    }
    if (variable === 'humn') {
      // = '3565000000000' // max
      // = '3560000000000' // min
      // I MANUALLY BINARY SEARCHED USING THIS VALUE OH MY FRACKIN DAYS
      eq = '3560324848168'
    }
    return { ...eqs, [variable]: eq }
  }, {})

let rootVal = null
// console.log(equations)

const isEquation = (variable) => /[=><\+\-\*\/]/.test(variable)

const splitEquation = (equation) => {
  const [lhsVar, op, rhsVar] = equation.split(' ')
  // console.log('isEquation', lhsVar, op, rhsVar)

  const lhs = equations[lhsVar]
  const rhs = equations[rhsVar]
  // console.log({ lhs, rhs, l: isEquation(lhs) })

  const lhsVal = isEquation(lhs) ? splitEquation(lhs) : lhs
  const rhsVal = isEquation(rhs) ? splitEquation(rhs) : rhs
  // console.log({ lhsVal, rhsVal })
  return { lhs: lhsVal, op, rhs: rhsVal }
  // return `(${lhsVal}${op}${rhsVal})`
}

const findEquationValue = (key) => {
  const equation = equations[key]
  if (isEquation(equation)) {
    return splitEquation(equation)
  } else {
    return equation
  }
}

// equations.humn = 'X'
let rootEquation = findEquationValue('root')

const solveEquation = (equation) => {
  // if (equation === 'X') {
  //   return equation
  // }
  const { lhs, op, rhs } = equation

  let lhsVal = lhs
  if (typeof lhsVal !== 'string') {
    lhsVal = solveEquation(lhsVal)
  } else {
    // is either a number or X
    if (lhsVal === 'X') {
      return equation
    }
  }
  let rhsVal = rhs
  if (typeof rhsVal !== 'string') {
    rhsVal = solveEquation(rhsVal)
  } else {
    // is either a number or X
    if (lhsVal === 'X') {
      return equation
    }
  }

  // console.log({lhs: typeof lhsVal, rhs: typeof rhsVal})
  if (typeof lhsVal === 'object' || typeof rhsVal === 'object') {
    return equation
  }
  // console.log('eval', `${eval(`${lhsVal}${op}${rhsVal}`)}`)
  return `${eval(`${lhsVal}${op}${rhsVal}`)}`
}
const hasHuman = (equation) => JSON.stringify(equation).includes('X')

const solveWithoutHuman = (equation) => {
  const { lhs, rhs } = equation
  // console.log('solve', {lhs, rhs})
  if (lhs !== 'X') {
    equation.lhs = solveEquation(lhs)
  }
  if (rhs !== 'X') {
    equation.rhs = solveEquation(rhs)
  }
  return equation
}

// now we simplify the equation as much as possible by reducing each lhs/rhs
// until we have solves sides excluding human value
// const rootLhs = rootEquation.lhs
// const rootRhs = rootEquation.rhs

// const sideWithoutHuman = hasHuman(rootLhs) ? rootRhs : rootLhs
// const solvedSideWithoutHuman = solveEquation(sideWithoutHuman)

rootEquation = solveEquation(rootEquation)
console.dir({ rootEquation }, { depth: null, colors: true })

// console.dir({ rootEquation }, { depth: null, colors: true })
console.log('Part two:', 'the value of X in the above equation')
