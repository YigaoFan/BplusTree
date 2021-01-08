const text = document.querySelector('#id-log')
const printCode = function(s) {
    text.value += s
    text.value += '\n'
}
const printTreeCode = function(root) {
    var leafIdx = 0
    var middleIdx = 0
    root.traverse(function (data, subNodeNames) {
        var name = null
        if (subNodeNames.length == 0) {
            name = `leafNode${leafIdx++}`
        } else {
            name = `middleNode${middleIdx++}`
        }
        printCode(`auto ${name} = LabelNode(${data}, { ${subNodeNames.join(', ')} });`)
        return name
    })
}
const printReadStateNodeCode = function(root) {
    var leafIdx = 0
    var middleIdx = 0
    root.traverseSubIf(n => n.read,
        function(node, subNodeNames) {
            var name = null
            var read = node.read || false
            if (subNodeNames.length == 0) {
                name = `leafNode${leafIdx++}`
            } else {
                name = `middleNode${middleIdx++}`
            }
            printCode(`auto ${name} = ReadStateLabelNode(${node.data}, { ${subNodeNames.join(', ')} },  ${read});`)
            return name
        })

}

const getWindow = function(id) {
    var canvas = document.querySelector(id)
    var context = canvas.getContext('2d')
    return {
        canvas,
        context,
    }
}

const __main = function() {
    var mainWindow = getWindow('#id-canvas')
    var entireControlIds = [
        '#id-entire-default-checkbox',
        '#id-entire-delete-checkbox',
        '#id-entire-move-checkbox',
        '#id-entire-part-checkbox',
    ]
    
    var root = new Node(0, 275, 20)
    var freeNode = new Node(0)
    mainEdit = new TreeEditUI(mainWindow, entireControlIds, root, freeNode)

    window.addEventListener('keydown', function (event) {
        var k = event.key
        mainEdit.onKeyDown(k)
    })
    window.addEventListener('keyup', function (event) {
        var k = event.key
        mainEdit.onKeyUp(k)
    })

    var genTreeButton = document.querySelector('#id-generate-tree-button')
    var genPartTreeButton = document.querySelector('#id-generate-part-tree-button')
    var genFreeNodeButton = document.querySelector('#id-generate-free-node-button')

    genTreeButton.addEventListener('click', function(event) {
        printCode('-------------root-------------')
        printTreeCode(root)
        printCode('')
        printCode('')
    })
    genPartTreeButton.addEventListener('click', function (event) {
        var partTree = mainEdit.getPartTree()
        if (partTree == null) {
            alert('please check part mode option, then select root of the part tree')
        } else {
            printCode('-------------part tree-------------')
            printReadStateNodeCode(partTree)
            printCode('')
            printCode('')
        }
    })
    genFreeNodeButton.addEventListener('click', function (event) {
        printCode('-------------free node-------------')
        printTreeCode(freeNode)
        printCode('')
        printCode('')
    })
    var clearButton = document.querySelector('#id-clear-button')
    clearButton.addEventListener('click', function (event) {
        text.value = ''
    })

    setInterval(function() {
        mainEdit.draw()
    }, 1000/30)
}

__main()