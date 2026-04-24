import L from 'leaflet';
import 'leaflet/dist/leaflet.css';


const map = L.map('map').setView([40.7692, -73.9866], 14);
L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
}).addTo(map);

let dijkstraLine = null;
let aStarLine = null;
let bellmanFordLine = null
let overlapLine = null;
let startMarker = null;
let endMarker = null;

function colorMarker(color) {
    return L.divIcon({
        className: '',
        html: `<div style="
            width: 14px;
            height: 14px;
            background: ${color};
            border-radius: 50%;
            box-shadow: 0 0 4px rgba(0,0,0,0.5);
        "></div>`,
        iconSize: [14, 14],
        iconAnchor: [7, 7]
    });
}

//haversine of the entire path
function pathDistance(path) {
    let total = 0;
    for (let i = 0; i < path.length - 1; i++) {
        const a = path[i], b = path[i + 1];
        const R = 6371;
        const dLat = (b.lat - a.lat) * Math.PI / 180;
        const dLon = (b.lon - a.lon) * Math.PI / 180;
        const x = Math.sin(dLat/2) * Math.sin(dLat/2) +
            Math.cos(a.lat * Math.PI / 180) * Math.cos(b.lat * Math.PI / 180) *
            Math.sin(dLon/2) * Math.sin(dLon/2);
        total += R * 2 * Math.asin(Math.sqrt(x));
    }
    return (total * 0.621371).toFixed(2); // km to miles
}

document.getElementById("find-button").addEventListener('click', () => {
    const sA = document.getElementById("streetA").value;
    const sB = document.getElementById("streetB").value;
    if (!sA || !sB) return;
    document.getElementById("result").innerHTML = "running algorithms..";

    Promise.all([
        fetch(`http://localhost:8000/dijkstra?streeta=${encodeURIComponent(sA)}&streetb=${encodeURIComponent(sB)}`).then(r => r.json()),
        fetch(`http://localhost:8000/aStar?streeta=${encodeURIComponent(sA)}&streetb=${encodeURIComponent(sB)}`).then(r => r.json()),
        fetch(`http://localhost:8000/bellmanFord?streeta=${encodeURIComponent(sA)}&streetb=${encodeURIComponent(sB)}`).then(r => r.json())
    ]).then(([dijkstra, aStar, bellmanFord]) => {
        document.getElementById("result").innerHTML =
            `<p>green dot represents src node, red represents target node</p>
             <p><br></p>
             <p><strong>dijkstra</strong> (blue): <br>| ${dijkstra.travelTime} minutes <br>| ${pathDistance(dijkstra.path)} miles <br>| run time: ${dijkstra.runTime} ms</p>
             <p><br></p>
             <p><strong>a*</strong> (red): <br>| ${aStar.travelTime} minutes <br>| ${pathDistance(aStar.path)} miles <br>| run time: ${aStar.runTime} ms</p>
             <p><br></p>
             <p><strong>bellman-ford</strong> (green): <br>| ${bellmanFord.travelTime} minutes <br>| ${pathDistance(aStar.path)} miles <br>| run time: ${bellmanFord.runTime} ms</p>
             <p><br></p>
             <p>(purple) = paths overlapping</p>`;

        // remove old lines
        if (dijkstraLine) map.removeLayer(dijkstraLine);
        if (aStarLine) map.removeLayer(aStarLine);
        if (overlapLine) map.removeLayer(overlapLine);
        if(bellmanFordLine) map.removeLayer(bellmanFordLine)

        const dijkstraCoords = dijkstra.path.map(p => [p.lat, p.lon]);
        const aStarCoords = aStar.path.map(p => [p.lat, p.lon]);
        const bellmanFordCoords = bellmanFord.path.map(p => [p.lat, p.lon]);

        // find overlapping segments by comparing node ids
        const dijkstraIds = new Set(dijkstra.path.map(p => p.id));
        const overlapIds = new Set(aStar.path.filter(p => dijkstraIds.has(p.id)).map(p => p.id));

        // build overlap coordinate segments
        const overlapCoords = aStar.path
            .filter(p => overlapIds.has(p.id))
            .map(p => [p.lat, p.lon]);

        // draw dijkstra in blue, a* in red
        dijkstraLine = L.polyline(dijkstraCoords, { color: 'blue', weight: 7 }).addTo(map);
        aStarLine = L.polyline(aStarCoords, { color: 'red', weight: 7 }).addTo(map);
        bellmanFordLine = L.polyline(bellmanFordCoords, {color: 'green', weight: 7}).addTo(map);

        // draw overlap in purple on top
        if (overlapCoords.length > 1) {
            //overlapLine = L.polyline(overlapCoords, { color: 'purple', weight: 7 }).addTo(map);
        }

        const group = L.featureGroup([dijkstraLine, aStarLine,bellmanFordLine]);
        map.fitBounds(group.getBounds());

        if (startMarker) map.removeLayer(startMarker);
        if (endMarker) map.removeLayer(endMarker);

        const startPoint = dijkstra.path[0];
        const endPoint = dijkstra.path[dijkstra.path.length - 1];

        startMarker = L.marker([startPoint.lat, startPoint.lon], { icon: colorMarker('#00ff88') })
            .bindPopup(`<strong>${startPoint.lat}, ${startPoint.lon}</strong>`).addTo(map);

        endMarker = L.marker([endPoint.lat, endPoint.lon], { icon: colorMarker('#e94560') })
            .bindPopup(`<strong>${endPoint.lat}, ${endPoint.lon}</strong>`).addTo(map);

    }).catch(err => {
        document.getElementById("result").innerText = `Error: ${err.message}`;
    });
});

/*
 * Loads all available streets when the page loads so that testers know the limit in which they display
 */
document.addEventListener("DOMContentLoaded", async () => {
    const res = await fetch("http://localhost:8000/streets");
    const data = await res.json();
    const selectA = document.getElementById("streetA");
    const selectB = document.getElementById("streetB");
    data.availableStreets.forEach(street => {
        const optA = document.createElement("option");
        optA.value = street;
        optA.textContent = street;
        selectA.appendChild(optA);
        const optB = document.createElement("option");
        optB.value = street;
        optB.textContent = street;
        selectB.appendChild(optB);
    });
});