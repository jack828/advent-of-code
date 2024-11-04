const assert = require('node:assert/strict')
const { join } = require('path')
const { readFileSync } = require('fs')

const raw = readFileSync(join(__dirname, './input.txt'), 'utf-8')
// const raw = readFileSync(join(__dirname, './example.txt'), 'utf-8')
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
        return 0;
      }
    }
    return 1;
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
let total = 0
for (let row of data) {
  const count = countArrangements(row)
  total += count
  console.log({ count })
}

assert(total < 13658)
console.log('part one', total)
// console.log(data)
