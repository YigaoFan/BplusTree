class EntireEdit extends EditBase {
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
    static get partMode() { 
        return 4
    }
    // externalModes 和 ids 是一一对应的
    static get externalModes() {
        return [
            EntireEdit.defaultMode,
            EntireEdit.deleteMode,
            EntireEdit.moveMode,
            EntireEdit.partMode,
        ]
    }

    constructor(window, showNode, freeNode, partEdit) {
        super(window, showNode)
        this.currentMode = EntireEdit.defaultMode;
        this.operateNode = null // node for drag, move and data edit use
        this.freeNode = freeNode
        this.partEdit = partEdit

        this.initCheckbox()
    }

    initCheckbox() {
        var o = this
        var checkboxes = []
        var ids = [
            '#id-default-checkbox',
            '#id-delete-checkbox',
            '#id-move-checkbox',
            '#id-part-checkbox',
        ]
        for (const id of ids) {
            var e = document.querySelector(id)
            checkboxes.push(e)
        }
        this.defaultCheckbox = checkboxes[0]
        this.deleteCheckbox = checkboxes[1]
        this.moveCheckbox = checkboxes[2]
        this.partCheckbox = checkboxes[3]

        for (let box of checkboxes) {
            box.addEventListener('change', () => {
                if (box.checked) {
                    o.currentMode = EntireEdit.externalModes[checkboxes.indexOf(box)]
                    var others = checkboxes.filter(e => e != box)
                    for (let x of others) {
                        x.checked = false
                    }
                } else {
                    this.defaultCheckbox.click()
                }
            })
        }

        this.defaultCheckbox.click()
        window.del = this.deleteCheckbox
    }

    onMouseMove = (x, y) => {
        var o = this
        if (o.currentMode == EntireEdit.dragMode && o.operateNode != null) {
            o.operateNode.x = x
            o.operateNode.y = y
        } else if (o.currentMode == EntireEdit.defaultMode) {
            // for edit operate data
            o.operateNode = root.locateNode(x, y)
        } else if (o.currentMode == EntireEdit.moveMode) {
            if (o.operateNode != null) {
                var n = o.operateNode
                n.x = x
                n.y = y
                if (n.parent != null) { // 更准确的说，root 不可能为 n，所以 n.parent 不可能为 0
                    n.parent.remove(n)
                    var newParent = root.searchClosestNode(x, y)
                    newParent.addChild(n)
                }
            }
        }
    }

    onMouseDown = (x, y) => {
        var o = this
        var n = root.locateNode(x, y)
        if (n != null) {
            if (o.currentMode == EntireEdit.deleteMode) {
                // delete on node，只删这点
                if (n.parent != null) {
                    n.parent.remove(n)
                    var c = n.giveChildren()
                    c.forEach(x => n.parent.addChild(x))
                    o.freeNode.addChild(n)
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
            var subNode = root.locateRelation(x, y)
            if (subNode != null) {
                if (o.currentMode == EntireEdit.deleteMode) {
                    // delete on line，都删
                    if (subNode.parent != null) {
                        subNode.parent.remove(subNode)
                        o.freeNode.addChild(subNode)
                    }
                } else if (o.currentMode == EntireEdit.defaultMode) {
                    var parent = subNode.parent
                    if (parent != null) {
                        var n = new Node(getData(), x, y)
                        o.operateNode = n
                        subNode.insertParent(n)
                        o.currentMode = EntireEdit.dragMode
                    }
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

    onKeyDown = key => {
        var o = this

        log('key', key)
        var k = key
        if (o.currentMode == EntireEdit.partMode) {
            o.partEdit.onKeyDown(k)
        } else {
            if (k == 'd' && !o.deleteCheckbox.checked) {
                o.deleteCheckbox.click()
            } if (k == 'e' && !o.moveCheckbox.checked) {
                o.moveCheckbox.click()
            } else if (o.currentMode == EntireEdit.defaultMode && o.operateNode != null) {
                if ('0123456789'.includes(k)) {
                    o.operateNode.data += k
                } else {
                    switch (k) {
                        case 'Backspace':
                            o.operateNode.data = o.operateNode.data.slice(0, -1)
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

        if (k == 'd' && o.deleteCheckbox.checked) {
            log('delete checkbox', o.deleteCheckbox)
            o.deleteCheckbox.click()
        } else if (k == 'e' && o.moveCheckbox.checked) {
            o.moveCheckbox.click()
        }
    }
}