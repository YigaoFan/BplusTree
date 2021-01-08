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

    // 感觉 PartEdit 也需要 freeNode
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

    onMouseMove = (x, y) => {
        var o = this
        if (o.currentMode == EntireEdit.dragMode && o.currentOperateNode != null) {
            o.currentOperateNode.x = x
            o.currentOperateNode.y = y
        } else if (o.currentMode == EntireEdit.defaultMode) {
            // for edit operate data
            o.currentOperateNode = this.showNode?.locateNode(x, y)
        } else if (o.currentMode == EntireEdit.moveMode) {
            var n = o.currentOperateNode
            if (n != null) {
                n.x = x
                n.y = y
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
                o.currentOperateNode = n.bornChild()
            } else if (o.currentMode == EntireEdit.moveMode) {
                o.currentOperateNode = n
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
                    var n = new Node(getDataStr(), x, y)
                    o.currentOperateNode = n
                    relationSubNode.insertParent(n)
                    o.currentMode = EntireEdit.dragMode
                }
            }
        }
    }

    onMouseUp = () => {
        var o = this

        if (o.currentOperateNode == null) {
            return
        }

        if (o.currentMode == EntireEdit.dragMode) {
            if (o.currentOperateNode.x == o.currentOperateNode.parent.x) {
                if (o.currentOperateNode.y == o.currentOperateNode.parent.y) {
                    o.currentOperateNode.parent.remove(o.currentOperateNode)
                }
            }
            o.currentOperateNode = null
            o.currentMode = EntireEdit.defaultMode
        } else if (o.currentMode == EntireEdit.moveMode) {
            o.currentOperateNode = null
            // move mode 的状态变化是由 checkbox 控制的，所以不用这里控制，和 EntireEdit.deleteMode 一样
            // o.currentMode = EntireEdit.defaultMode
        }
    }

    draw() {
        super.draw()
        var s = 'mode: ' + this.currentMode.toString()
        if (this.currentOperateNode != null) {
            s += (' node: ' + this.currentOperateNode.data.toString())
        }
        this.window.context.textAlign = "left"
        this.window.context.font = '20px Arial'
        this.window.context.fillText(s, 8, 20)
    }

    partEditOnKeyDown = key => {
        if (this.currentOperateNode == null) {
            return
        }
        
        var k = key
        var n = this.currentOperateNode
        if (k == 'r') {
            n.read = n.read == null ? true : !n.read
        } else {
            this.onKeyDown(k)
        }
    }

    onKeyDown = key => {
        var o = this

        log('key', key)
        var k = key
        if (o.currentMode == EntireEdit.partMode) {
            o.partEdit.partEditOnKeyDown(k)
        } else {
            if (k == 'd' && !o.deleteCheckbox.checked) {
                o.deleteCheckbox.click()
            } if (k == 'e' && !o.moveCheckbox.checked) {
                o.moveCheckbox.click()
            } else if (o.currentMode == EntireEdit.defaultMode && o.currentOperateNode != null) {
                if ('0123456789'.includes(k)) {
                    o.currentOperateNode.data += k
                } else {
                    switch (k) {
                        case 'Backspace':
                            log()
                            o.currentOperateNode.data = o.currentOperateNode.data.slice(0, -1)
                            break;
                        case 'e':
                            break;
                    }
                }
            }
        }
    }

    onKeyUp = key => {
        var o = this
        var k = key

        if (o.currentMode == EntireEdit.partMode) {
            o.partEdit.onKeyUp(k)
        } else if (k == 'd' && o.deleteCheckbox.checked) {
            o.deleteCheckbox.click()
        } else if (k == 'e' && o.moveCheckbox.checked) {
            o.moveCheckbox.click()
        }
    }
}