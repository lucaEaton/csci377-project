import L from 'leaflet';
import 'leaflet/dist/leaflet.css';


const map = L.map('map').setView([40.7692, -73.9866], 14);
L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {}).addTo(map);

let dijkstraLine = [];
let aStarLine = [];
let bellmanFordLine = [];
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
        const x = Math.sin(dLat / 2) * Math.sin(dLat / 2) +
            Math.cos(a.lat * Math.PI / 180) * Math.cos(b.lat * Math.PI / 180) *
            Math.sin(dLon / 2) * Math.sin(dLon / 2);
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
             <p>(purple): all paths overlapping</p>
             <p>(orange): dijkstra + a* overlapping</p>
             <p>(gold): dijkstra + bellman overlapping</p>
             <p>(orange): a* + bellman overlapping</p>`;

        [...dijkstraLine, ...aStarLine, ...bellmanFordLine].forEach(l => l && map.removeLayer(l));
        dijkstraLine = [];
        aStarLine = [];
        bellmanFordLine = [];
        // building a set of node keys for each path that utilize the lat n lon between the current n next node
        const dEdges = new Set();
        for (let i = 0; i < dijkstra.path.length - 1; i++)
            dEdges.add(`${dijkstra.path[i].lat},${dijkstra.path[i].lon}-${dijkstra.path[i + 1].lat},${dijkstra.path[i + 1].lon}`);

        const aEdges = new Set();
        for (let i = 0; i < aStar.path.length - 1; i++)
            aEdges.add(`${aStar.path[i].lat},${aStar.path[i].lon}-${aStar.path[i + 1].lat},${aStar.path[i + 1].lon}`);

        const bEdges = new Set();
        for (let i = 0; i < bellmanFord.path.length - 1; i++)
            bEdges.add(`${bellmanFord.path[i].lat},${bellmanFord.path[i].lon}-${bellmanFord.path[i + 1].lat},${bellmanFord.path[i + 1].lon}`);


        let dCoords = [[dijkstra.path[0].lat, dijkstra.path[0].lon]];
        let dColor = 'blue';
        //build da path coloring
        for (let i = 0; i < dijkstra.path.length - 1; i++) {
            // grab the lat n long
            const edge = `${dijkstra.path[i].lat},${dijkstra.path[i].lon}-${dijkstra.path[i + 1].lat},${dijkstra.path[i + 1].lon}`;
            // see if it also exists within a* and bellman path
            const inA = aEdges.has(edge), inB = bEdges.has(edge);
            // depending on if it does appear or not provide its respected color to such.
            const nextColor = (inA && inB) ? 'purple' : inA ? 'orange' : inB ? 'goldenrod' : 'blue';
            // if it doesn't make its own color, then provided it, add it to the polyline and
            if (nextColor !== dColor) {
                dijkstraLine.push(L.polyline(dCoords, {
                    color: dColor,
                    weight: 7,
                    opacity: dColor === 'blue' ? 0.8 : 1
                }).addTo(map));
                dCoords = [[dijkstra.path[i].lat, dijkstra.path[i].lon]];
                dColor = nextColor;
            }
            dCoords.push([dijkstra.path[i + 1].lat, dijkstra.path[i + 1].lon]);
        }
        dijkstraLine.push(L.polyline(dCoords, {
            color: dColor,
            weight: 7,
            opacity: dColor === 'blue' ? 0.8 : 1
        }).addTo(map));

        let aCoords = [[aStar.path[0].lat, aStar.path[0].lon]];
        let aColor = 'red';
        for (let i = 0; i < aStar.path.length - 1; i++) {
            // grab current "key"
            const edge = `${aStar.path[i].lat},${aStar.path[i].lon}-${aStar.path[i + 1].lat},${aStar.path[i + 1].lon}`;
            // check if the array holds this key either T or F
            const inD = dEdges.has(edge), inB = bEdges.has(edge);
            //determine what color
            const nextColor = (inD && inB) ? 'purple' : inD ? 'orange' : inB ? 'cyan' : 'red';
            // depending on if that's our main color, we will push a poly line
            if (nextColor !== aColor) {
                aStarLine.push(L.polyline(aCoords, {
                    color: aColor,
                    weight: 7,
                    opacity: aColor === 'red' ? 0.8 : 1
                }).addTo(map));
                aCoords = [[aStar.path[i].lat, aStar.path[i].lon]];
                aColor = nextColor;
            }
            aCoords.push([aStar.path[i + 1].lat, aStar.path[i + 1].lon]);
        }
        aStarLine.push(L.polyline(aCoords, {
            color: aColor,
            weight: 7,
            opacity: aColor === 'red' ? 0.8 : 1
        }).addTo(map));

        let bCoords = [[bellmanFord.path[0].lat, bellmanFord.path[0].lon]];
        let bColor = 'green';
        for (let i = 0; i < bellmanFord.path.length - 1; i++) {
            const edge = `${bellmanFord.path[i].lat},${bellmanFord.path[i].lon}-${bellmanFord.path[i + 1].lat},${bellmanFord.path[i + 1].lon}`;
            const inD = dEdges.has(edge), inA = aEdges.has(edge);
            const nextColor = (inD && inA) ? 'purple' : inD ? 'goldenrod' : inA ? 'cyan' : 'green';
            if (nextColor !== bColor) {
                bellmanFordLine.push(L.polyline(bCoords, {
                    color: bColor,
                    weight: 7,
                    opacity: bColor === 'green' ? 0.8 : 1
                }).addTo(map));
                bCoords = [[bellmanFord.path[i].lat, bellmanFord.path[i].lon]];
                bColor = nextColor;
            }
            bCoords.push([bellmanFord.path[i + 1].lat, bellmanFord.path[i + 1].lon]);
        }
        bellmanFordLine.push(L.polyline(bCoords, {
            color: bColor,
            weight: 7,
            opacity: bColor === 'green' ? 0.8 : 1
        }).addTo(map));

        const group = L.featureGroup([...dijkstraLine, ...aStarLine, ...bellmanFordLine]);
        map.fitBounds(group.getBounds());

        if (startMarker) map.removeLayer(startMarker);
        if (endMarker) map.removeLayer(endMarker);

        // assuming they all start n end the same spot
        const startPoint = dijkstra.path[0];
        const endPoint = dijkstra.path[dijkstra.path.length - 1];

        startMarker = L.marker([startPoint.lat, startPoint.lon], {icon: colorMarker('#00ff88')})
            .bindPopup(`<strong>${startPoint.lat}, ${startPoint.lon}</strong>`).addTo(map);

        endMarker = L.marker([endPoint.lat, endPoint.lon], {icon: colorMarker('#e94560')})
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