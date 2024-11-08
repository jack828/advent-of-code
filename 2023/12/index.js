const assert = require('node:assert/strict')
const { join } = require('path')
const { readFileSync } = require('fs')

// const raw = readFileSync(join(__dirname, './input.txt'), 'utf-8')
const raw = readFileSync(join(__dirname, './example.txt'), 'utf-8')
const data = raw
  .split('\n')
  .filter(Boolean)
  .map((row) => {
    const [springs, groups] = row.split(' ')
    return [springs, groups.split(',').map(Number)]
  })

const countArrangements = (input) => {
  const [line, groups] = input
  // console.log(
  // `count: '${line}', len ${line.length}, group_count ${groups.length}`
  // )

  if (line.length == 0) {
    // if we don't have groups, this is GOOD solution
    if (groups.length == 0) {
      // console.log('1\n')
      return 1
    }
    // otherwise, we still had groups to process, BAD solution
    // console.log('0 - groups left\n')
    return 0
  }

  if (groups.length == 0) {
    for (let i = 0; i < line.length; i++) {
      if (line[i] == '#') {
        // it isnt, return early
        // printf("0 - no fit\n");
        return 0
      }
    }
    return 1
  }

  // if it starts with a ., discard the . and recursively check again.
  if (line[0] == '.') {
    // console.log('dot recurse')
    const newline = line.slice(1)
    return countArrangements([newline, [...groups]])
  }

  // if it starts with a ?, replace the ? with a . and recursively check again,
  // AND replace it with a # and recursively check again.
  if (line[0] == '?') {
    // console.log('? recurse')
    const line_a = '.' + line.slice(1)
    const line_b = '#' + line.slice(1)

    let a_count = countArrangements([line_a, [...groups]])
    let b_count = countArrangements([line_b, [...groups]])
    // freeInput(input_a);
    // freeInput(input_b);
    // return a_count;
    return a_count + b_count
  }

  // it it starts with a #,
  // check if it is long enough for the first group,
  // check if all characters in the first [grouplength] characters are not '.',
  // and then remove the first [grouplength] chars and the first group number,
  // recursively check again.
  if (line[0] == '#') {
    // check if it is long enough for the first group,
    for (let i = 0; i < groups[0]; i++) {
      if (line[i] == '.') {
        // it isnt, return early
        // console.log('0 - no fit\n')
        return 0
      }
    }
    if (line[groups[0]] == '#') {
      // console.log('0 - no fit?\n')
      return 0
    }

    // and then remove the first [grouplength] chars and the first group
    // number,
    const newline = line.slice(groups[0] + 1)
    const newgroups = groups.slice(1)
    // console.log('# recurse')
    // recursively check again.
    let count = countArrangements([newline, newgroups])
    // freeInput(new_input);
    return count
  }
  // console.log('BAD\n')
  return 0
}
const isValid = (line, groups) => {
  const springs = line.split(/\.+/).filter(Boolean)
  if (springs.length !== groups.length) {
    return false
  }
  // console.log(springs)
  return springs.every((s, i) => s.length === groups[i])
}

// does one level of arrangements
const getArrangements = (line, i) => {
  if (i > line.length) {
    return line
  }
  let char = line[i]
  if (char !== '?') {
    return getArrangements(line, i + 1)
  }
  let a = line.substring(0, i) + '#' + line.substring(i + 1)
  let b = line.substring(0, i) + '.' + line.substring(i + 1)

  return [getArrangements(a, i + 1), getArrangements(b, i + 1)].flat()
}
const countArrangements2 = ([line, groups]) => {
  console.log(line, groups)
  isValid(line, groups)

  let arrangements = getArrangements(line, 0)
    .flat()
    .filter((option) => isValid(option, groups))
  // console.log(arrangements)
  return arrangements.length
}

let totalPartOne = 0
for (let row of data) {
  // const count = countArrangements(row)
  const count = countArrangements2(row)
  // return
  totalPartOne += count
  console.log({ count })
}

// assert(totalPartOne == 8270)
console.log('part one', totalPartOne)
// console.log(data)
const partTwoData = data.map(([springs, groups]) => [
  Array(5).fill(springs).join('?'),
  Array(5).fill(groups).flat()
])
// console.log(partTwoData)
let totalPartTwo = 0
for (let row of partTwoData) {
  // const count = countArrangements(row)
  const count = countArrangements2(row)
  console.log(count)
  // return
  totalPartTwo += count
  console.log({ count })
}
console.log(totalPartTwo)
