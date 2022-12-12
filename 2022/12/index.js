const { join } = require('path')
const { readFileSync } = require('fs')

const input = readFileSync(join(__dirname, './input.txt'), 'utf-8')
// const input = `Sabqponm
// abcryxxl
// accszExk
// acctuvwj
// abdefghi`
let startpos = [0, 0]
let endpos = [0, 0]
let startingPoints = []
const grid = input
  .split('\n')
  .filter(Boolean)
  .map((row, i) =>
    row.split('').map((col, j) => {
      if (col === 'E') {
        endpos = [i, j]
        col = 'z'
      }
      if (col === 'S') {
        startpos = [i, j]
        col = 'a'
      }
      if (col === 'a') {
        startingPoints.push([i, j])
      }
      return col.charCodeAt(0) - 96
    })
  )
const WIDTH = grid[0].length
const HEIGHT = grid.length
const LENGTH = WIDTH * HEIGHT

class Node {
  constructor(val, priority) {
    this.val = val
    this.priority = priority
  }
}

class PriorityQueue {
  constructor() {
    this.values = []
  }
  enqueue(val, priority) {
    let newNode = new Node(val, priority)
    this.values.push(newNode)
    this.bubbleUp()
  }
  bubbleUp() {
    let idx = this.values.length - 1
    const element = this.values[idx]
    while (idx > 0) {
      let parentIdx = Math.floor((idx - 1) / 2)
      let parent = this.values[parentIdx]
      if (element.priority >= parent.priority) break
      this.values[parentIdx] = element
      this.values[idx] = parent
      idx = parentIdx
    }
  }
  dequeue() {
    const min = this.values[0]
    const end = this.values.pop()
    if (this.values.length > 0) {
      this.values[0] = end
      this.sinkDown()
    }
    return min
  }
  sinkDown() {
    let idx = 0
    const length = this.values.length
    const element = this.values[0]
    while (true) {
      let leftChildIdx = 2 * idx + 1
      let rightChildIdx = 2 * idx + 2
      let leftChild, rightChild
      let swap = null

      if (leftChildIdx < length) {
        leftChild = this.values[leftChildIdx]
        if (leftChild.priority < element.priority) {
          swap = leftChildIdx
        }
      }
      if (rightChildIdx < length) {
        rightChild = this.values[rightChildIdx]
        if (
          (swap === null && rightChild.priority < element.priority) ||
          (swap !== null && rightChild.priority < leftChild.priority)
        ) {
          swap = rightChildIdx
        }
      }
      if (swap === null) break
      this.values[idx] = this.values[swap]
      this.values[swap] = element
      idx = swap
    }
  }
}

class WeightedGraph {
  constructor() {
    this.adjacencyList = {}
  }
  addVertex(vertex) {
    if (!this.adjacencyList[vertex]) this.adjacencyList[vertex] = []
  }
  addEdge(vertex1, vertex2, weight) {
    if (weight === Infinity) {
      return
    }
    this.adjacencyList[vertex1].push({ node: vertex2, weight: weight })
    // Importantly, this isn't allowed
    // this.adjacencyList[vertex2].push({ node: vertex1, weight: weight })
  }
  Dijkstra(start, finish) {
    const nodes = new PriorityQueue()
    const distances = {}
    const previous = {}
    let path = [] //to return at end
    let smallest
    //build up initial state
    for (let vertex in this.adjacencyList) {
      if (vertex === start) {
        distances[vertex] = 0
        nodes.enqueue(vertex, 0)
      } else {
        distances[vertex] = Infinity
        nodes.enqueue(vertex, Infinity)
      }
      previous[vertex] = null
    }
    // as long as there is something to visit
    while (nodes.values.length) {
      smallest = nodes.dequeue().val
      if (smallest === finish) {
        //WE ARE DONE
        //BUILD UP PATH TO RETURN AT END
        while (previous[smallest]) {
          path.push(smallest)
          smallest = previous[smallest]
        }
        break
      }
      if (smallest || distances[smallest] !== Infinity) {
        for (let neighbour in this.adjacencyList[smallest]) {
          //find neighbouring node
          let nextNode = this.adjacencyList[smallest][neighbour]
          //calculate new distance to neighbouring node
          let candidate = distances[smallest] + nextNode.weight
          let nextNeighbour = nextNode.node
          if (candidate < distances[nextNeighbour]) {
            //updating new smallest distance to neighbour
            distances[nextNeighbour] = candidate
            //updating previous - How we got to neighbour
            previous[nextNeighbour] = smallest
            //enqueue in priority queue with new priority
            nodes.enqueue(nextNeighbour, candidate)
          }
        }
      }
    }
    return path.concat(smallest).reverse()
  }
}

const graph = new WeightedGraph()

for (let i = 0; i < LENGTH; i++) {
  let y = Math.floor(i / WIDTH)
  let x = i % WIDTH
  graph.addVertex(`${y}:${x}`)
}


for (let i = 0; i < LENGTH; i++) {
  let y = Math.floor(i / WIDTH)
  let x = i % WIDTH
  let current = grid[y][x]
  // for each edge find the neighbours and their weight
  // if the neighbour isnt reachable then set to Infinity
  // otherwise just weight of 1

  let weight
  let nextY
  let nextX
  // up
  if (y > 0) {
    nextY = y - 1
    nextX = x
    weight = grid[nextY][nextX] <= current + 1 ? 1 : Infinity
    graph.addEdge(`${y}:${x}`, `${nextY}:${nextX}`, weight)
  }

  // down
  if (y < HEIGHT - 1) {
    nextY = y + 1
    nextX = x
    weight = grid[nextY][nextX] <= current + 1 ? 1 : Infinity
    graph.addEdge(`${y}:${x}`, `${nextY}:${nextX}`, weight)
  }

  // left
  if (x > 0) {
    nextY = y
    nextX = x - 1
    weight = grid[nextY][nextX] <= current + 1 ? 1 : Infinity
    graph.addEdge(`${y}:${x}`, `${nextY}:${nextX}`, weight)
  }

  // right
  if (x < WIDTH - 1) {
    nextY = y
    nextX = x + 1
    weight = grid[nextY][nextX] <= current + 1 ? 1 : Infinity

    graph.addEdge(`${y}:${x}`, `${nextY}:${nextX}`, weight)
  }
}

const path = graph.Dijkstra(
  `${startpos[0]}:${startpos[1]}`,
  `${endpos[0]}:${endpos[1]}`
)
console.log('Part one:', path.length - 1)

const paths = startingPoints
  .map(
    (point) =>
      graph.Dijkstra(`${point[0]}:${point[1]}`, `${endpos[0]}:${endpos[1]}`)
        .length - 1
  )
  .filter((path) => path > 0)
  .sort((a, b) => (a > b ? 1 : -1))

console.log(paths)
