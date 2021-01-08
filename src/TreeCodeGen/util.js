const log = console.log.bind(console)

const distance = function(x1, y1, x2, y2) {
    return Math.sqrt(Math.pow(x1 - x2, 2) + Math.pow(y1 - y2, 2))
}

/// point is { x, y }
/// line is [point1, point2]
const distanceToLine = function(point, line) {
    var p1 = point
    var p2 = line[0]
    var p3 = line[1]
    var A = distance(p1.x, p1.y, p2.x, p2.y)
    var B = distance(p1.x, p1.y, p3.x, p3.y)
    var C = distance(p2.x, p2.y, p3.x, p3.y)
    // 利用海伦公式计算三角形面积
    var P = (A + B + C) / 2
    var area = Math.sqrt(P * (P - A) * (P - B) * (P - C))

    var dis = (2 * area) / C
    return dis
}

const getDataStr = function () {
    getDataStr.n = getDataStr.n ? getDataStr.n + 1 : 1;
    return getDataStr.n.toString()
}