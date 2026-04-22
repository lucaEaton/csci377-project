import L from 'leaflet';
import 'leaflet/dist/leaflet.css';


const map = L.map('map').setView([40.7692, -73.9866], 14);
L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
}).addTo(map);

let routeLine = null;

document.getElementById("find-button").addEventListener('click', () => {
    const sA = document.getElementById("streetA").value;
    const sB = document.getElementById("streetB").value;
    if (!sA || !sB) return;

    fetch(`http://localhost:8000/dijkstra?streeta=${encodeURIComponent(sA)}&streetb=${encodeURIComponent(sB)}`)
        .then(r => r.json())
        .then(data => {
            document.getElementById("result").innerHTML =
                `<strong>Result:</strong> ${data.travelTime} minutes`;

            // remove old route if exists
            if (routeLine) map.removeLayer(routeLine);

            // draw the path on the map
            const coords = data.path.map(p => [p.lat, p.lon]);
            routeLine = L.polyline(coords, { color: 'blue', weight: 4 }).addTo(map);
            map.fitBounds(routeLine.getBounds());
        })
        .catch(err => {
            document.getElementById("result").innerText = `Error: ${err.message}`;
        });
});