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
}