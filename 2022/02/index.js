const { join } = require('path')
const {readFileSync} = require('fs')

const input = readFileSync(join(__dirname, './input.txt'), 'utf-8')
// const input = `A Y
// B X
// C Z`

const games = input.split('\n').filter(Boolean).map(game => game.split(' '))
console.log(games)

//
//X for Rock, Y for Paper, and Z for Scissors.
//1 for Rock, 2 for Paper, and 3 for Scissors)
const moveScoreMap = {
  X: 1, // Rock
  Y: 2, // Paper
  Z: 3, // Scissors
}

const opponentMoveMap = {
  A: 'X',
  B: 'Y',
  C: 'Z'
}

// A for Rock, B for Paper, and C for Scissors.
// (0 if you lost, 3 if the round was a draw, and 6 if you won).
const checkResult = (opponent, me) => {
opponent = opponentMoveMap[opponent]
  if (opponent ===  me) {
    return 3 // d
  }
  if (opponent === 'X' && me === 'Y') {
    return 6
  }
  if (opponent === 'X' && me === 'Z') {
    return 0
  }
  if (opponent === 'Y' && me === 'X') {
    return 0
  }
  if (opponent === 'Y' && me === 'Z') {
    return 6
  }
  if (opponent === 'Z' && me === 'X') {
    return 6
  }
  if (opponent === 'Z' && me === 'Y') {
    return 0
  }
  return 1e11
}
let score = 0
games.forEach(([opponent, me])=> {
  // console.log(opponent, me)
  score += moveScoreMap[me] + checkResult(opponent, me)
})

console.log('Part one score: ', score)

// opponent - A for Rock, B for Paper, and C for Scissors.
//    me    - X for Rock, Y for Paper, and Z for Scissors.
// (0 if you lost, 3 if the round was a draw, and 6 if you won).
const moveMap = {
  A: { // Rock
    X: 'Z',
    Y: 'X',
    Z: 'Y'
  },
  B: { // Paper
    X: 'X',
    Y: 'Y',
    Z: 'Z'
  },
  C: { // Scissors
    X: 'Y',
    Y: 'Z',
    Z: 'X'
  }
}
// X means you need to lose, Y means you need to end the round in a draw, and Z means you need to win. Good luck!
const resultScoreMap = {
  X: 0,
  Y: 3,
  Z: 6
}

let score2 = 0
games.forEach(([opponent, outcome])=> {
  const myMove = moveMap[opponent][outcome]
  // console.log(opponent, outcome, myMove)
  score2 += moveScoreMap[myMove] + resultScoreMap[outcome]
})
console.log('Part two score: ', score2)
