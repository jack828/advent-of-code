const { join } = require('path')
const { readFileSync } = require('fs')

const input = readFileSync(join(__dirname, './input.txt'), 'utf-8')
const inputtest = `    [D]
[N] [C]
[Z] [M] [P]
 1   2   3

move 1 from 2 to 1
move 3 from 1 to 3
move 2 from 2 to 1
move 1 from 1 to 2`
const [stackInput, movesInput] = input.split('\n\n').filter(Boolean)

const columns = stackInput.split('\n').map((row) => {
  const out = []
  for (let i = 0; i < row.length; i += 4) {
    out.push(row.slice(i, i + 4))
  }
  return out.map((item) => item.replace(/[\W]/g, ''))
})
const indexes = columns.pop()
// console.log(columns)
// console.log(indexes)
const stack = columns.reduce(
  (cols, col) => {
    // console.log({ cols })
    // console.log({ col })
    for (let index in indexes) {
      // console.log({ index })
      if (col[index]) {
        cols[Number(index) + 1] = [col[index]].concat(cols[Number(index) + 1])
      }
    }
    return cols
  },
  indexes.reduce((idxs, idx) => ({ ...idxs, [idx]: [] }), {})
)

const stack2 = Object.entries(stack).reduce(
  (newStack, [key, value]) => ({ ...newStack, [key]: [...value] }),
  {}
)
console.log('stack', stack)
console.log('stack2', stack2)

console.log(movesInput)
// const moves = movesInput
// .split('\n')
// .filter(Boolean)
// .map((row) =>
// /[a-z ]+(\d+)[a-z ]+(\d+)[a-z ]+(\d+)/.exec(row).slice(1, 4).map(Number)
// )
const moves = movesInput
  .split('\n')
  .filter(Boolean)
  .map((row) => {
    const [, count, , from, , to] = row.split(' ')
    console.log(1, count, from, to)
    return [count, from, to]
  })
console.log(moves)

for (let move of moves) {
  const [count, from, to] = move
  console.log(count, from, to)
  for (let i = 0; i < count; i++) {
    const el = stack[from].pop()
    stack[to].push(el)
  }
}

console.log(stack)

const topElements = Object.values(stack).map((col) => [...col].pop())

console.log('Part one:', topElements.join(''))

for (let move of moves) {
  const [count, from, to] = move
  const elementsToMove = []
  for (let i = 0; i < count; i++) {
    elementsToMove.push(stack2[from].pop())
  }
  stack2[to] = stack2[to].concat(elementsToMove.reverse())
}

console.log(stack2)

const topElements2 = Object.values(stack2).map((col) => [...col].pop())
console.log('Part two:', topElements2.join(''))
