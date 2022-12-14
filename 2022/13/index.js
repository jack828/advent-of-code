const { join } = require('path')
const { readFileSync } = require('fs')

const input = readFileSync(join(__dirname, './input.txt'), 'utf-8')
// const input = `[1,1,3,1,1]
// [1,1,5,1,1]
//
// [[1],[2,3,4]]
// [[1],4]
//
// [9]
// [[8,7,6]]
//
// [[4,4],4,4]
// [[4,4],4,4,4]
//
// [7,7,7,7]
// [7,7,7]
//
// []
// [3]
//
// [[[]]]
// [[]]
//
// [1,[2,[3,[4,[5,6,7]]]],8,9]
// [1,[2,[3,[4,[5,6,0]]]],8,9]`

const groups = input
  .split('\n\n')
  .filter(Boolean)
  .map((group) => group.split('\n').map((row) => eval(row)))
const inputRows = input
  .split('\n')
  .filter(Boolean)
  .map((row) => eval(row))

let rightOrderIndexes = []

const compare = (left, right) => {
  // If both values are integers,
  if (Number.isInteger(left) && Number.isInteger(right)) {
    // the lower integer should come first.
    // If the left integer is lower than the right integer,
    // the inputs are in the right order.
    if (left < right) {
      return true
    }
    // If the left integer is higher than the right integer,
    // the inputs are not in the right order.
    if (left > right) {
      return false
    }
    // Otherwise, the inputs are the same integer;
    // continue checking the next part of the input.
    return null // continue
  }

  // If both values are lists,
  if (Array.isArray(left) && Array.isArray(right)) {
    // compare the first value of each list, then the second value, and so on.
    // If the right list runs out of items first, the inputs are not in the right order.
    // If the lists are the same length and no comparison makes a decision about the order,
    // continue checking the next part of the input.
    for (let i = 0; i < Math.max(left.length, right.length); i++) {
      // If the left list runs out of items first, the inputs are in the right order.
      if (typeof left[i] === 'undefined') {
        return true
      }
      if (typeof right[i] === 'undefined') {
        return false
      }
      let result = compare(left[i], right[i])
      if (result !== null) {
        return result
      }
    }
    return null
  }

  // If exactly one value is an integer,
  // convert the integer to a list which contains that integer as its only value,
  // then retry the comparison.
  // For example, if comparing [0,0,0] and 2, convert the right value to [2] (a list containing 2); the result is then found by instead comparing [0,0,0] and [2].
  if (Array.isArray(left) && Number.isInteger(right)) {
    return compare(left, [right])
  }
  if (Number.isInteger(left) && Array.isArray(right)) {
    return compare([left], right)
  }
}

groups.forEach(([left, right], index) => {
  if (compare(left, right)) {
    rightOrderIndexes.push(index + 1)
  }
})

const rightOrderIndiciesSum = rightOrderIndexes.reduce(
  (total, i) => total + i,
  0
)
console.log('Part one:', rightOrderIndiciesSum)
const dividers = [[[2]], [[6]]]
const sortedRows = [...inputRows, ...dividers].sort((a, b) =>
  compare(a, b) ? -1 : 1
)

const dividerIndexes = dividers.map((divider) =>
  sortedRows.findIndex((row) => JSON.stringify(row) === JSON.stringify(divider)) + 1
)
const partTwo = dividerIndexes[0] * dividerIndexes[1]
console.log('Part two:', partTwo)
