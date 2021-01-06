class PartEdit extends EditBase {
    constructor(window) {
        super(window, null)
        this.currentOperateNode = null
    }

    onMouseMove = (x, y) => {
        // 如果不用箭头函数，下面这行就读 showNode 就是 undefined，奇怪，那为什么 setNode 就可以设置成功呢
        // 应该和之前听说的 this 的问题相关，可能当前这个方法在回调里，this 就变了，不像自己认为的那样了
        if (this.showNode == null) {
            return
        }

        var n = this.showNode.locateNode(x, y)
        this.currentOperateNode = n
    }

    /// deep copy node
    setNode(node) {
        this.showNode = node.clone()
    }

    onKeyDown(key) {
        if (this.currentOperateNode == null) {
            return
        }
        
        var k = key
        var n = this.currentOperateNode
        if (k == 'r') {
            n.read = n.read == null ? true : !n.read
        } else if (k == 'd') {
            n.deleted = n.deleted == null ? true : !n.deleted
        }
    }
}