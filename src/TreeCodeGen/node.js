const Node = function (data, x = null, y = null, parent = null) {
    var o = {
        x,
        y,
        width: 50,
        height: 50,
        data,
        children: [],
        parent,
    }

    o.bornChild = function () {
        var c = Node(++data, o.x, o.y, o)
        o.children.push(c)
        return c
    }

    o.locateNode = function (x, y) {
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

    o.draw = function (context) {
        context.fillStyle = 'black'
        context.strokeRect(o.x, o.y, o.width, o.height)
        context.fillStyle = 'green'
        context.fillRect(o.x, o.y, o.width, o.height)
        context.beginPath()
        context.fillStyle = 'black'
        context.font = '20px Arial'
        context.textAlign = "center"
        var p1 = o.getMiddlePoint()
        context.fillText(o.data, p1.x, p1.y)

        o.children.forEach(c => {
            c.draw(context)
            context.moveTo(p1.x, p1.y)
            context.fillStyle = 'black'
            var p2 = c.getMiddlePoint()
            context.lineTo(p2.x, p2.y)
            context.stroke()
        })
    }

    o.getMiddlePoint = function () {
        return {
            x: (o.x + o.x + o.width) / 2,
            y: (o.y + o.y + o.height) / 2,
        }
    }

    o.traverse = function (callbackOnLeaf, callbackOnMiddle) {
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

    o.remove = function (child) {
        o.children = o.children.filter(e => {
            return e != child
        })
        log("remove: ", child)
        log('child count after remove: ', o.children.length)
    }

    o.addChild = function(node) {
        o.children.push(node)
        o.children.sort((a, b) => a.data - b.data)
    }

    return o
}