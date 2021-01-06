const EditMode = function(window, root, freeNode, partEdit) {
    const defaultMode = 0 // 加节点和修改节点数据的
    const deleteMode = 1
    const dragMode = 2
    const moveMode = 3
    const partMode = 4
    // externalModes 和 ids 是一一对应的
    const externalModes = [
        defaultMode,
        deleteMode,
        moveMode,
        partMode,
    ]
    var o = {
        currentMode: defaultMode,
        checkboxes: [],
        operateNode: null, // node for drag, move and data edit use
    }
    
    var ids = [
        '#id-default-checkbox',
        '#id-delete-checkbox',
        '#id-move-checkbox',
        '#id-part-checkbox',
    ]
    for (const id of ids) {
        var e = document.querySelector(id)
        o.checkboxes.push(e)
    }
    var defaultCheckbox = o.checkboxes[0]
    var deleteCheckbox = o.checkboxes[1]
    var moveCheckbox = o.checkboxes[2]
    var partCheckbox = o.checkboxes[3]

    for (let box of o.checkboxes) {
        box.addEventListener('change', function (event) {
            if (box.checked) {
                o.currentMode = externalModes[o.checkboxes.indexOf(box)]
                var others = o.checkboxes.filter(e => e != box)
                for (let x of others) {
                    x.checked = false
                }
            } else {
                defaultCheckbox.click()
            }
        })
    }

    defaultCheckbox.click()

    window.canvas.addEventListener('mousedown', function (event) {
        var x = event.offsetX
        var y = event.offsetY
        o.onMouseDown(x, y)
    })
    window.canvas.addEventListener('mouseup', function (event) {
        o.onMouseUp()
    })
    window.canvas.addEventListener('mousemove', function (event) {
        var x = event.offsetX
        var y = event.offsetY
        o.onMouseMove(x, y)
    })

    o.onMouseMove = function(x, y) {
        if (o.currentMode == dragMode && o.operateNode != null) {
            o.operateNode.x = x
            o.operateNode.y = y
        } else if (o.currentMode == defaultMode) {
            // for edit operate data
            o.operateNode = root.locateNode(x, y)
        } else if (o.currentMode == moveMode) {
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

    o.onMouseDown = function(x, y) {
        var n = root.locateNode(x, y)
        if (n != null) {
            if (o.currentMode == deleteMode) {
                // delete on node，只删这点
                if (n.parent != null) {
                    n.parent.remove(n)
                    var c = n.giveChildren()
                    c.forEach(x => n.parent.addChild(x))
                    freeNode.addChild(n)
                }
            } else if (o.currentMode == defaultMode) {
                o.currentMode = dragMode
                o.operateNode = n.bornChild()
            } else if (o.currentMode == moveMode) {
                o.operateNode = n
            } else if (o.currentMode == partMode) {
                partEdit.copyAsPartNode(n)
            }
        } else {
            var subNode = root.locateRelation(x, y)
            if (subNode != null) {
                if (o.currentMode == deleteMode) {
                    // delete on line，都删
                    if (subNode.parent != null) {
                        subNode.parent.remove(subNode)
                        freeNode.addChild(subNode)
                    }
                } else if (o.currentMode == defaultMode) {
                    var parent = subNode.parent
                    if (parent != null) {
                        var n = new Node(getData(), x, y)
                        o.operateNode = n
                        subNode.insertParent(n)
                        o.currentMode = dragMode
                    }
                }
            }
        }
    }

    o.onMouseUp = function() {
        if (o.operateNode != null) {
            if (o.currentMode == dragMode) {
                if (o.operateNode.x == o.operateNode.parent.x) {
                    if (o.operateNode.y == o.operateNode.parent.y) {
                        o.operateNode.parent.remove(o.operateNode)
                    }
                }
                o.operateNode = null
                o.currentMode = defaultMode
            } else if (o.currentMode == moveMode) {
                o.operateNode = null
                // move mode 的状态变化是由 checkbox 控制的，所以不用这里控制，和 deleteMode 一样
                // o.currentMode = defaultMode
            }
        }
    }

    o.onKeyDown = function(key) {
        log('key', key)
        var k = key
        if (o.currentMode == partMode) {
            partEdit.onKeyDown(k)
        } else {
            if (k == 'd' && !deleteCheckbox.checked) {
                deleteCheckbox.click()
            } if (k == 'e' && !moveCheckbox.checked) {
                moveCheckbox.click()
            } else if (o.currentMode == defaultMode && o.operateNode != null) {
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

    o.onKeyUp = function(key) {
        var k = key
        if (k == 'd' && deleteCheckbox.checked) {
            deleteCheckbox.click()
        } else if (k == 'e' && moveCheckbox.checked) {
            moveCheckbox.click()
        }
    }

    o.draw = function() {
        window.context.clearRect(0, 0, 600, 600)
        root.draw(window.context)
    }

    return o
}
