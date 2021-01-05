const log = console.log.bind(console)
const text = document.querySelector('#id-log')
const printCode = function(s) {
    text.value += s
    text.value += '\n'
}
const printTreeCode = function (root) {
    var leafIdx = 0
    var middleIdx = 0
    root.traverse(function (data, subNodeNames) {
        var name = null
        if (subNodeNames.length == 0) {
            name = `leafNode${leafIdx++}`
            printCode(`auto ${name} = LabelNode(${data});`)
        } else {
            name = `middleNode${middleIdx++}`
            printCode(`auto ${name} = LabelNode(${data}, { ${subNodeNames.join(', ')} });`)
        }
        return name
    })
}
const getData = function() {
    getData.n = getData.n ? getData.n + 1 : 1;
    return getData.n
}

const __main = function() {
    var canvas = document.querySelector('#id-canvas')
    var context = canvas.getContext('2d')
    var showNodes = []
    
    var root = Node(0, 375, 20)
    showNodes.push(root)
    var freeNode = Node(0)
    var mode = EditMode(root, freeNode, showNodes)
    
    canvas.addEventListener('mousedown', function(event) {
        var x = event.offsetX
        var y = event.offsetY
        mode.onMouseDown(x, y)
    })
    canvas.addEventListener('mouseup', function (event) {
        mode.onMouseUp()
    })
    canvas.addEventListener('mousemove', function (event) {
        var x = event.offsetX
        var y = event.offsetY
        mode.onMouseMove(x, y)
    })
    window.addEventListener('keydown', function (event) {
        var k = event.key
        mode.onKeyDown(k)
    })
    window.addEventListener('keyup', function (event) {
        var k = event.key
        mode.onKeyUp(k)
    })

    var generateButton = document.querySelector('#id-generate-button')

    var levels = [
        {
            name: 'Store original tree state',
            handler : function (event) {
                printCode("original:")
                printTreeCode(root)
            }
        },
        {
            name: 'Generate tree code after change',
            handler : function (event) {
                printCode("after modify:")
                printTreeCode(root)

                printCode("delete nodes:")
                printTreeCode(freeNode)
            }
        }
    ]

    var currentLevel = 0
    generateButton.innerText = levels[currentLevel].name
    generateButton.addEventListener('click', function(event) {
        levels[currentLevel].handler(event)
        ++currentLevel
        if (!(currentLevel < levels.length)) {
            currentLevel = 0
            printCode('')
            printCode('')
        }

        generateButton.innerText = levels[currentLevel].name
    })
    var clearButton = document.querySelector('#id-clear-button')
    clearButton.addEventListener('click', function (event) {
        text.value = ''
        freeNode.children = []
    })

    setInterval(function() {
        context.clearRect(0, 0, 800, 600)
        showNodes.forEach(e => e.draw(context))
    }, 1000/30)
}

__main()