class EditBase {
    constructor(window, showNode) {
        this.window = window
        this.showNode = showNode

        this.initListener()
    }

    initListener() {
        var window = this.window
        var o = this
        var callWhenExist = (method, ...arg) => {
            if (method != null) {
                method(...arg)
            }
        }

        window.canvas.addEventListener('mousedown', function(event) {
            var x = event.offsetX
            var y = event.offsetY
            callWhenExist(o.onMouseDown, x, y)
        })
        window.canvas.addEventListener('mouseup', function(event) {
            callWhenExist(o.onMouseUp)
        })
        window.canvas.addEventListener('mousemove', function(event) {
            var x = event.offsetX
            var y = event.offsetY
            callWhenExist(o.onMouseMove, x, y)
        })
    }

    draw() {
        var window = this.window
        var canvas = window.canvas
        window.context.clearRect(0, 0, canvas.width, canvas.height)
        this.showNode.draw(window.context)
    }
}