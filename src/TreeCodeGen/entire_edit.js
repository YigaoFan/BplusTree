class EntireEdit extends PartEdit {
    static get partMode() {
        return 4
    }

    constructor(window, showNode, freeNode, partEdit, modeControlIds) {
        super(window, modeControlIds, showNode)

        PartEdit.externalModes.push(EntireEdit.partMode)
        this.partCheckbox = this.checkboxes[3]
        this.freeNode = freeNode
        this.partEdit = partEdit
        // remove freeNode, partEdit in PartEdit class TODO
    }

    onKeyDown = key => {
        var o = this

        // TODO 视鼠标位置分发给不同的 window
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
            o.deleteCheckbox.click()
        } else if (k == 'e' && o.moveCheckbox.checked) {
            o.moveCheckbox.click()
        }
    }
}