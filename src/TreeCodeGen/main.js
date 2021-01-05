const log = console.log.bind(console)
const text = document.querySelector('#id-log')
const printCode = function(s) {
    text.value += s
    text.value += '\n'
}

const __main = function() {
    var canvas = document.querySelector('#id-canvas')
    var context = canvas.getContext('2d')

    var root = Node(0, 375, 20)
    var freeNode = Node(0)

    const normalMode = 0
    const deleteMode = 1
    const dragMode = 2
    var mode = normalMode
    
    var operateNode = null
    canvas.addEventListener('mousedown', function(event) {
        var x = event.offsetX
        var y = event.offsetY

        var n = root.locateNode(x, y)
        if (n != null) {
            if (mode == deleteMode) {
                if (n != root) {
                    n.parent.remove(n)
                    freeNode.addChild(n)
                }
            } else if (mode == normalMode) {
                mode = dragMode
                operateNode = n.bornChild()
            }
        }
    })
    canvas.addEventListener('mouseup', function (event) {
        if (mode == dragMode && operateNode != null) {
            if (operateNode.x == operateNode.parent.x) {
                if (operateNode.y == operateNode.parent.y) {
                    operateNode.parent.remove(operateNode)
                }
            }
            operateNode = null
            mode = normalMode
        }
    })
    canvas.addEventListener('mousemove', function (event) {
        var x = event.offsetX
        var y = event.offsetY
        
        if (mode == dragMode && operateNode != null) {
            operateNode.x = x
            operateNode.y = y
        } else if (mode == normalMode) {
            operateNode = root.locateNode(x, y)
        }
    })
    window.addEventListener('keydown', function (event) {
        var k = event.key
        if (k == 'd') {
            mode = deleteMode
        } else if ('0123456789'.includes(k)) {
            if (mode == normalMode && operateNode != null) {
                operateNode.data = k
            }
        }
    })
    window.addEventListener('keyup', function (event) {
        var k = event.key
        if (k == 'd') {
            mode = normalMode
        }
    })

    var b = document.querySelector('#id-generate-button')
    var printTreeCode = function(tree) {
        var leafIdx = 0
        var middleIdx = 0
        tree.traverse(function (data, subNodeNames) {
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
    b.innerText = levels[currentLevel].name

    b.addEventListener('click', function(event) {
        levels[currentLevel].handler(event)
        ++currentLevel
        if (!(currentLevel < levels.length)) {
            currentLevel = 0
            printCode('')
            printCode('')
        }

        b.innerText = levels[currentLevel].name
    })
    var clearButton = document.querySelector('#id-clear-button')
    clearButton.addEventListener('click', function (event) {
        text.value = ''
        freeNode.children = []
    })

    setInterval(function() {
        context.clearRect(0, 0, 800, 600)
        root.draw(context)
    }, 1000/30)
}

__main()