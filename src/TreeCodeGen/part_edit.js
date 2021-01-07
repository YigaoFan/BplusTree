class PartEdit extends EditBase {
    // 加节点和修改节点数据的
    static get defaultMode() {
        return 0
    }
    static get deleteMode() {
        return 1
    }
    static get dragMode() {
        return 2
    }
    static get moveMode() {
        return 3
    }
    // externalModes 和 ids 是一一对应的
    static externalModes = [
        PartEdit.defaultMode,
        PartEdit.deleteMode,
        PartEdit.moveMode,
    ]

    constructor(window, modeControlIds, showNode = null) {
        super(window, showNode)

        this.currentOperateNode = null // node for drag, move and data edit use
        this.currentMode = EntireEdit.defaultMode;
        this.initCheckbox(modeControlIds)
    }

    initCheckbox(ids) {
        var o = this
        this.checkboxes = []
        for (const id of ids) {
            var e = document.querySelector(id)
            o.checkboxes.push(e)
        }
        this.defaultCheckbox = o.checkboxes[0]
        this.deleteCheckbox = o.checkboxes[1]
        this.moveCheckbox = o.checkboxes[2]

        for (let box of o.checkboxes) {
            box.addEventListener('change', () => {
                if (box.checked) {
                    o.currentMode = EntireEdit.externalModes[o.checkboxes.indexOf(box)]
                    var others = o.checkboxes.filter(e => e != box)
                    for (let x of others) {
                        x.checked = false
                    }
                } else {
                    this.defaultCheckbox.click()
                }
            })
        }

        this.defaultCheckbox.click()
    }

    /// deep copy node
    setNode(node) {
        this.showNode = node.clone()
    }

    // 可以增删移、修改是否读（指的状态）
    onMouseMove = (x, y) => {
        var o = this
        if (o.currentMode == EntireEdit.dragMode && o.operateNode != null) {
            o.operateNode.x = x
            o.operateNode.y = y
        } else if (o.currentMode == EntireEdit.defaultMode) {
            // for edit operate data
            o.operateNode = this.showNode?.locateNode(x, y)
        } else if (o.currentMode == EntireEdit.moveMode) {
            var n = o.operateNode
            if (n != null) {
                n.x = x
                n.y = y
                log('move node', n)
                if (n.parent != null) {
                    n.parent.remove(n)
                    var newParent = this.showNode?.searchClosestNode(x, y)
                    newParent.addChild(n)
                }
            }
        }
    }

    onMouseDown = (x, y) => {
        var o = this
        var n = this.showNode?.locateNode(x, y)
        if (n != null) {
            if (o.currentMode == EntireEdit.deleteMode) {
                // delete on node，只删这点
                if (n.parent != null) {
                    n.parent.remove(n)
                    var c = n.giveChildren()
                    c.forEach(x => n.parent.addChild(x))
                    o.freeNode?.addChild(n)
                }
            } else if (o.currentMode == EntireEdit.defaultMode) {
                o.currentMode = EntireEdit.dragMode
                o.operateNode = n.bornChild()
            } else if (o.currentMode == EntireEdit.moveMode) {
                o.operateNode = n
            } else if (o.currentMode == EntireEdit.partMode) {
                o.partEdit.setNode(n)
            }
        } else {
            var relationSubNode = this.showNode?.locateRelation(x, y)
            if (relationSubNode != null) {
                if (o.currentMode == EntireEdit.deleteMode) {
                    // delete on line，都删
                    relationSubNode.parent.remove(relationSubNode)
                    o.freeNode?.addChild(relationSubNode)
                } else if (o.currentMode == EntireEdit.defaultMode) {
                    var n = new Node(getData(), x, y)
                    o.operateNode = n
                    relationSubNode.insertParent(n)
                    o.currentMode = EntireEdit.dragMode
                }
            }
        }
    }

    onMouseUp = () => {
        var o = this

        if (o.operateNode == null) {
            return
        }

        if (o.currentMode == EntireEdit.dragMode) {
            if (o.operateNode.x == o.operateNode.parent.x) {
                if (o.operateNode.y == o.operateNode.parent.y) {
                    o.operateNode.parent.remove(o.operateNode)
                }
            }
            o.operateNode = null
            o.currentMode = EntireEdit.defaultMode
        } else if (o.currentMode == EntireEdit.moveMode) {
            o.operateNode = null
            // move mode 的状态变化是由 checkbox 控制的，所以不用这里控制，和 EntireEdit.deleteMode 一样
            // o.currentMode = EntireEdit.defaultMode
        }
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
            // 这里可以不要了 TODO
            n.deleted = n.deleted == null ? true : !n.deleted
        }
    }
}