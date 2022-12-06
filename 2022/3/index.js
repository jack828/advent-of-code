const { join } = require('path')
const { readFileSync } = require('fs')

const input = readFileSync(join(__dirname, './input.txt'), 'utf-8')
// const input = `vJrwpWtwJgWrhcsFMMfFFhFp
// jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
// PmmdzqPrVvPwwTWBwg
// wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
// ttgJtRGJQctTZtZT
// CrZsJsPPZsGzwwsLwLmpwMDw`
const inputRows = input.split('\n').filter(Boolean)

const rucksacks = inputRows.map((row) => [
  row.slice(0, row.length / 2),
  row.slice(row.length / 2)
])
console.log(rucksacks)

const duplicates = rucksacks.map(
  ([one, two]) => one.split('').filter((item) => two.includes(item))[0]
)
console.log(duplicates)

const priority = (char) => {
  if (char >= 'a' && char <= 'z')
    return char.charCodeAt(0) - 'a'.charCodeAt(0) + 1
  if (char >= 'A' && char <= 'Z')
    return char.charCodeAt(0) - 'A'.charCodeAt(0) + 27
}

console.log(
  priority('a'), // 1
  priority('z'), // 26
  priority('A'), // 27
  priority('Z') // 52
)

const prioritySum = duplicates.reduce(
  (total, letter) => total + priority(letter),
  0
)
console.log('Part one sum:', prioritySum)

const groups = []

for (let i = 0; i < inputRows.length; i += 3) {
  groups.push(inputRows.slice(i, i + 3))
}
console.log(groups)

const duplicatesThrees = groups.map(
  ([one, two, three]) =>
    one
      .split('')
      .filter((item) => two.includes(item))
      .filter((item) => three.includes(item))[0]
)
console.log(duplicatesThrees)

const prioritySum2 = duplicatesThrees.reduce(
  (total, letter) => total + priority(letter),
  0
)
console.log('Part two sum:', prioritySum2)
