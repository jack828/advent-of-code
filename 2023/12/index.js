const assert = require('node:assert/strict')
const { join } = require('path')
const { readFileSync } = require('fs')

let test_mode = true
test_mode = false
const raw = test_mode
  ? readFileSync(join(__dirname, './example.txt'), 'utf-8')
  : readFileSync(join(__dirname, './input.txt'), 'utf-8')
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
  const springs = line.split('.').filter(Boolean)
  if (springs.length !== groups.length) {
    return false
  }
  // console.log(springs)
  return springs.every((s, i) => s.length === groups[i])
}

const cache = new Map()
const getArrangements = (line, i) => {
  if (i === line.length) {
    return [line]
  }
  let char = line[i]
  if (char !== '?') {
    return getArrangements(line, i + 1)
  }
  let start = line.substring(0, i)
  let end = line.substring(i + 1)
  let end_arrs
  let a = start + '#'
  let b = start + '.'

  // if (cache.has(end)) {
  //   end_arrs = cache.get(end)
  //   console.log('hit')
  // } else {
  end_arrs = getArrangements(end, i - start.length)
  // cache.set(end, end_arrs)
  // }

  return end_arrs.map((arr) => [a + arr, b + arr]).flat()
  return [getArrangements(a, i + 1), getArrangements(b, i + 1)].flat()
}

const countArrangements2 = ([line, groups]) => {
  isValid(line, groups)

  let arrangements = getArrangements(line, 0)
    .flat()
    .filter((option) => isValid(option, groups))
  // console.log(arrangements)
  return arrangements.length
}

/* let totalPartOnea = 0
for (let row of data) {
  // const count = countArrangements(row)
  const count = countArrangements2(row)
  // return
  totalPartOnea += count
}

assert(totalPartOne == 8270)
console.log('part one', totalPartOnea)
*/

console.time('startp1')

// thanks to
// https://old.reddit.com/r/adventofcode/comments/18hg99r/2023_day_12_simple_tutorial_with_memoization/
// and
// https://github.com/iggyzuk/advent-of-code/blob/master/2023/day-12-2023/src/bin/part1.rs#L135
let can_fit = (springs, start, end) => {
  // let all_chars = springs.chars().collect::<Vec<_>>();
  // // make sure the range's end fits into the springs string
  // //  XXXXX---] – good
  // //  XXX]X !   – bad
  if (end > springs.length) {
    return false
  }
  // // make sure that all chars in range are either a '?' or '#' – not '.'
  //
  let substr = springs.substring(start, end).includes('.')
  // console.log(substr)
  if (substr) {
    return false
  }
  // // make sure the next char is one of: { out_of_bounds, '.', '?' } – not '#'
  // let next = range.end;
  if (end < springs.length && springs[end] == '#') {
    return false
  }

  return true
}

const solve = (springs, groups, cache, i) => {
  // console.log(springs.substring(i), groups, i)
  //     if num groups is 0:
  if (groups.length === 0) {
    //         if any '#' remaining in springs return 0
    if (i < springs.length && springs.substring(i).includes('#')) {
      return 0
    } else {
      //         else return 1
      return 1
    }
  }

  //     advance i to the next available '?' or '#'
  for (; i < springs.length; i++) {
    if (springs[i] !== '.') {
      break
    }
  }

  //     if i > length of springs return 0
  if (i >= springs.length) {
    return 0
  }
  const key = `${i},${groups.length}`
  //     if (i, num groups) is in cache, return it
  if (cache.has(key)) {
    return cache.get(key)
  }

  let result = 0

  //     if we can fill the springs at i with the first group in groups:
  if (can_fit(springs, i, i + groups[0])) {
    //         recursively call with the groups after that at index: i + groupsize + 1
    result += solve(springs, groups.slice(1), cache, i + groups[0] + 1)
  }
  //
  //     if the current spot is '?':
  if (springs[i] === '?') {
    //         recursively call with current groups at the next index
    result += solve(springs, groups, cache, i + 1)
  }
  //
  //     add the result to the cache
  cache.set(key, result)
  return result
}

let totalPartOne = 0
for (let row of data) {
  // const count = countArrangements(row)
  const [line, groups] = row
  const count = solve(line, groups, new Map(), 0)
  // console.log({ count })
  // return
  totalPartOne += count
}
console.log('part one', totalPartOne)
console.timeEnd('startp1')

if (test_mode) {
  assert.equal(solve('???.###', [1, 1, 3], new Map(), 0), 1)
  assert.equal(solve('.??..??...?##.', [1, 1, 3], new Map(), 0), 4)
  assert.equal(solve('?#?#?#?#?#?#?#?', [1, 3, 1, 6], new Map(), 0), 1)
  assert.equal(solve('????.#...#...', [4, 1, 1], new Map(), 0), 1)
  assert.equal(solve('????.######..#####.', [1, 6, 5], new Map(), 0), 4)
  assert.equal(solve('?###????????', [3, 2, 1], new Map(), 0), 10)
  assert.equal(solve('?.?', [1, 1], new Map(), 0), 1)
  assert.equal(solve('?', [1], new Map(), 0), 1)
  assert.equal(solve('...', [1, 1, 1], new Map(), 0), 0)
  assert.equal(solve('.......?.....#?', [1, 2], new Map(), 0), 1)
  assert.equal(solve('?????', [1], new Map(), 0), 5)
  assert.equal(solve('?????', [1, 1], new Map(), 0), 6)
  assert.equal(solve('???????', [1, 5], new Map(), 0), 1)
  assert.equal(solve('?????', [1, 2], new Map(), 0), 3)
  assert.equal(solve('??????#??#??#??', [1, 2, 9], new Map(), 0), 4)
  assert(totalPartOne == 21)
} else {
  assert(totalPartOne == 8270)
}

console.time('startp2')
// console.log(data)
const partTwoData = data.map(([springs, groups]) => [
  Array(5).fill(springs).join('?'),
  Array(5).fill(groups).flat()
])
// console.log(partTwoData)
// return
let totalPartTwo = 0
for (let row of partTwoData) {
  // const count = countArrangements(row)
  const [line, groups] = row
  // const count = countArrangements2(row)
  const count = solve(line, groups, new Map(), 0)
  // console.log(count)
  // return
  totalPartTwo += count
  // console.log({ count })
}
console.log(totalPartTwo)
console.timeEnd('startp2')
if (test_mode) {
  assert(totalPartTwo === 525152)
} else {
  assert(totalPartTwo == 204640299929836)
}
