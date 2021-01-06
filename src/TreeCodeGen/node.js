class Node {
    constructor(data, x = null, y = null, parent = null) {
        this.data = data
        this.x = x
        this.y = y
        this.parent = parent

        this.width = 50
        this.height = 50
        this.children = []
    }

    bornChild() {
        var n = new Node(getData(), this.x, this.y, this)
        this.children.push(n)
        this.children.sort((a, b) => a.data - b.data)
        return n
    }

    locateNode(x, y) {
        var o = this
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

    locateRelation(x, y) {
        var o = this
        var distanceLimit = 5
        var p1 = o.getMiddlePoint()
        for (const c of o.children) {
            var p2 = c.getMiddlePoint()
            if (distanceToLine({ x, y, }, [p1, p2,]) < distanceLimit) {
                return c
            }

            var n = c.locateRelation(x, y)
            if (n != null) {
                return n
            }
        }

        return null
    }

    draw(context) {
        var o = this
        context.fillStyle = 'black'
        context.strokeRect(o.x, o.y, o.width, o.height)
        context.fillStyle = 'green'
        context.fillRect(o.x, o.y, o.width, o.height)
        context.beginPath()
        context.fillStyle = 'black'
        context.font = '20px Arial'
        context.textAlign = "center"
        var p1 = o.getMiddlePoint()

        var s = o.data.toString()
        if (o.deleted == true) {
            s += ' d'
        }
        if (o.read == true) {
            s += ' r'
        }
        context.fillText(s, p1.x, p1.y)

        o.children.forEach(c => {
            c.draw(context)
            context.moveTo(p1.x, p1.y)
            context.fillStyle = 'black'
            var p2 = c.getMiddlePoint()
            context.lineTo(p2.x, p2.y)
            context.stroke()
        })
    }

    getMiddlePoint() {
        var o = this
        return {
            x: (o.x + o.x + o.width) / 2,
            y: (o.y + o.y + o.height) / 2,
        }
    }

    traverse(callback) {
        var o = this
        var subResults = []
        o.children.forEach(e => {
            var r = e.traverse(callback)
            subResults.push(r)
        })

        return callback(o.data, subResults)
    }

    remove(child) {
        var o = this
        o.children = o.children.filter(e => {
            return e != child
        })
    }

    addChild(node) {
        var o = this
        node.parent = o
        o.children.push(node)
        o.children.sort((a, b) => a.data - b.data)
    }

    insertParent(node) {
        var o = this
        o.parent.remove(o)
        o.parent.addChild(node)
        node.addChild(o)
    }

    giveChildren() {
        var o = this
        var c = o.children
        o.children = []
        return c
    }

    searchClosestNode(x, y) {
        var o = this
        var p = o.getMiddlePoint()
        var minDistanceNode = o
        var minDistance = distance(x, y, p.x, p.y)

        for (let c of o.children) {
            var n = c.searchClosestNode(x, y)
            var d = distance(n.x, n.y, x, y)
            if (d < minDistance) {
                minDistance = d
                minDistanceNode = n
            }
        }

        return minDistanceNode
    }

    clone() {
        var o = this
        var children = []
        o.children.forEach(x => children.push(x.clone()))

        var n = new Node(o.data, o.x, o.y)
        n.children = children
        return n
    }
}