const { join } = require('path')
const { readFileSync } = require('fs')

const input = readFileSync(join(__dirname, './input.txt'), 'utf-8')
// const input = `2-4,6-8
// 2-3,4-5
// 5-7,7-9
// 2-8,3-7
// 6-6,4-6
// 2-6,4-8`
const inputRows = input.split('\n').filter(Boolean)

const groups = inputRows.map((row) =>
  row.split(',').map((col) => col.split('-').map(Number))
)

console.log(groups)

const contains = (source, target) =>
  source[0] >= target[0] && source[1] <= target[1]

console.log(contains([2, 8], [3, 7])) // no
console.log(contains([3, 7], [2, 8])) // yes
console.log(contains([6, 6], [4, 6])) // yes
console.log(contains([123, 456], [222, 800])) // no
console.log(contains([123, 456], [123, 789])) // yes

const fullyOverlappedGroups = groups.filter(
  ([a, b]) => contains(a, b) || contains(b, a)
)
console.log('Part one:', fullyOverlappedGroups.length)

const overlaps = (source, target) =>
  Math.max(...source) >= Math.min(...target)

// no
console.log(overlaps([2,4],[6,8]))
console.log(overlaps([2,3],[4,5]))
// yes
console.log(overlaps([5,7],[7,9]))
console.log(overlaps([2,8],[3,7]))
console.log(overlaps([6,6],[4,6]))
console.log(overlaps([2,6],[4,8]))


const partialOverlappedGroups = groups.filter(
  ([a, b]) => overlaps(a, b) && overlaps(b, a)
)
console.log(partialOverlappedGroups)
console.log('Part two:', partialOverlappedGroups.length)
