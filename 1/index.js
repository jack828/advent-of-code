const { join } = require('path')
const {readFileSync} = require('fs')

const input = readFileSync(join(__dirname, './input.txt'), 'utf-8')

const rows = input.split('\n')
// console.log(rows)

const entries = []
let currentEntry = 0
rows.forEach((row) => {
  if (!row && currentEntry) {
    entries.push(currentEntry)
    // console.log(currentEntry)
    currentEntry = 0
  }
  // console.log(row)
  currentEntry += Number(row)
})

console.log(entries)
const sortedEntries = entries.sort((a, b) => b-a)
console.log('Part one:', sortedEntries[0])
const topThree = sortedEntries.slice(0, 3).reduce((acc, row) => acc + row)
console.log(sortedEntries.slice(0,3), topThree)
console.log('Part two:', topThree)
