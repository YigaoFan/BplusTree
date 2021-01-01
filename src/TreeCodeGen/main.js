// const log = console.log.bind(console)
const log = function(s) {
    var text = document.querySelector('#id-log')
    text.value += s
    text.value += '\n'
}

const Node = function(x, y, data) {
    var o = {
        x,
        y,
        width: 50,
        height: 50,
        data,
        children: [],
    }

    o.bornChild = function() {
        var dx = Math.random() * 50 - 25
        var dy = Math.random() * 30 + 50
        var c = Node(o.x, o.y, ++data)
        // log('born new, ', o.x + o.width, o.y + o.height)
        o.children.push(c)
        return c
    }

    o.locateNode = function(x, y) {
        if (x >= o.x && x <= o.x + o.width) {
            if (y >= o.y && y <= o.y + o.height) {
                return o
            }
        }

        for (var i = 0; i < o.children.length; ++i) {
            var c = o.children[i]
            var n = c.locateNode(x, y)
            if (n != null) {
                return n
            }
        }

        return null
    }

    o.draw = function(context) {
        context.fillStyle = 'black'
        context.strokeRect(o.x, o.y, o.width, o.height)
        context.fillStyle = 'green'
        context.fillRect(o.x, o.y, o.width, o.height)
        context.beginPath()
        
        o.children.forEach(c => {
            c.draw(context)
            var p1 = o.getMiddlePoint()
            context.moveTo(p1.x, p1.y)
            context.fillStyle = 'black'
            var p2 = c.getMiddlePoint()
            context.lineTo(p2.x, p2.y)
            context.stroke()
        })
    }
    o.getMiddlePoint = function() {
        return {
            x: (o.x + o.x + o.width) / 2, 
            y: (o.y + o.y + o.height) / 2,
        }
    }
    o.traverse = function(callbackOnLeaf, callbackOnMiddle) {
        if (o.children.length == 0) {
            return callbackOnLeaf(o.data)
        } else {
            var subResults = []
            o.children.forEach(e => {
                var r = e.traverse(callbackOnLeaf, callbackOnMiddle)
                subResults.push(r)
            })

            return callbackOnMiddle(o.data, subResults)
        }
    }

    return o
}

const __main = function() {
    var canvas = document.querySelector('#id-canvas')
    context = canvas.getContext('2d')
    var root = Node(375, 20, 0)

    var opNode = null
    canvas.addEventListener('mousedown', function(event) {
        var x = event.offsetX
        var y = event.offsetY
        // log('pos: ', x, y)

        var n = root.locateNode(x, y)
        if (n != null) {
            opNode = n.bornChild()
        }
    })
    canvas.addEventListener('mouseup', function (event) {
        opNode = null
    })
    canvas.addEventListener('mousemove', function (event) {
        if (opNode != null) {
            var x = event.offsetX
            var y = event.offsetY
            opNode.x = x
            opNode.y = y
        }
    })
    var b = document.querySelector('#id-generate-button')
    b.addEventListener('click', function(event) {
        var leafIdx = 0
        var middleIdx = 0
        root.traverse(function(data) {
            var name = `leafNode${leafIdx++}`
            log(`auto ${name} = LabelNode(${data});`)
            return name
        }, 
        function(data, subResults) {
            var name = `middleNode${middleIdx++}`
            // var sub = subResults.concat()
            log(`auto ${name} = LabelNode(${data}, { ${subResults.join(', ')} });`)
            return name
        })
    })
    setInterval(function() {
        context.clearRect(0, 0, 800, 600)
        root.draw(context)
    }, 1000/30);
}

__main()