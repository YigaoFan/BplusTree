const PartEdit = function(window) {
    var o = {
        node: null,
        operateNode: null,
    }

    window.canvas.addEventListener('mousemove', function(event) {
        var x = event.offsetX
        var y = event.offsetY
        var node = o.node
        if (node != null) {
            var n = node.locateNode(x, y)
            if (n != null) {
                var m = n.getMiddlePoint()
            }
            o.operateNode = n
        }
    })

    o.copyAsPartNode = function(node) {
        o.node = node.clone()
    }

    o.onKeyDown = function(key) {
        var k = key
        var n = o.operateNode
        if (n != null) {
            log('part edit', n.data)
            if (k == 'r') {
                n.read = n.read == null ? true : !n.read
            } else if (k == 'd') {
                n.deleted = n.deleted == null ? true : !n.deleted
            }
        }
    }

    o.draw = function() {
        var context = window.context
        context.clearRect(0, 0, 600, 600)
        if (o.node != null) {
            o.node.draw(context)
        }
    }

    return o
}