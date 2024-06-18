function writeData() {
    fetch('https://api.thingspeak.com/update?api_key=1K8OS7SCUKG2UXCE&field1=0')
        .then(response => response.text())
        .then(data => {
            document.getElementById('response').innerHTML = 'Write response: ' + data;
        })
        .catch(error => console.error('Error writing data:', error));
}

function readData() {
    fetch('https://api.thingspeak.com/channels/2545003/feeds.json?api_key=YW5XKP20X5TNFB36&results=20')
        .then(response => response.json())
        .then(data => {
            const feeds = data.feeds;
            const labels = feeds.map(feed => new Date(feed.created_at).toLocaleString());
            const temperatureData = feeds.map(feed => parseFloat(feed.field2)); // Misalnya field2 adalah temperature
            const humidityData = feeds.map(feed => parseFloat(feed.field1)); // Misalnya field1 adalah humidity

            // Update tabel utama
            const tableBody = document.getElementById('data-table').getElementsByTagName('tbody')[0];
            tableBody.innerHTML = ''; // Clear previous data

            // Update tabel pagi
            const pagiTableBody = document.getElementById('pagi-table').getElementsByTagName('tbody')[0];
            pagiTableBody.innerHTML = ''; // Clear previous data

            // Update tabel siang
            const siangTableBody = document.getElementById('siang-table').getElementsByTagName('tbody')[0];
            siangTableBody.innerHTML = ''; // Clear previous data

            // Update tabel malam
            const malamTableBody = document.getElementById('malam-table').getElementsByTagName('tbody')[0];
            malamTableBody.innerHTML = ''; // Clear previous data

            feeds.forEach(feed => {
                const row = tableBody.insertRow();
                const timeCell = row.insertCell(0);
                const dateCell = row.insertCell(1);
                const tempCell = row.insertCell(2);
                const humidityCell = row.insertCell(3);
                
                const dateTime = new Date(feed.created_at);
                const hours = dateTime.getHours();

                timeCell.innerHTML = dateTime.toLocaleTimeString();
                dateCell.innerHTML = dateTime.toLocaleDateString();
                tempCell.innerHTML = feed.field2; // Temperature
                humidityCell.innerHTML = feed.field1; // Humidity

                if (hours >= 6 && hours < 12) { // Pagi
                    const pagiRow = pagiTableBody.insertRow();
                    pagiRow.insertCell(0).innerHTML = timeCell.innerHTML;
                    pagiRow.insertCell(1).innerHTML = dateCell.innerHTML;
                    pagiRow.insertCell(2).innerHTML = tempCell.innerHTML;
                    pagiRow.insertCell(3).innerHTML = humidityCell.innerHTML;
                } else if (hours >= 12 && hours < 18) { // Siang
                    const siangRow = siangTableBody.insertRow();
                    siangRow.insertCell(0).innerHTML = timeCell.innerHTML;
                    siangRow.insertCell(1).innerHTML = dateCell.innerHTML;
                    siangRow.insertCell(2).innerHTML = tempCell.innerHTML;
                    siangRow.insertCell(3).innerHTML = humidityCell.innerHTML;
                } else { // Malam
                    const malamRow = malamTableBody.insertRow();
                    malamRow.insertCell(0).innerHTML = timeCell.innerHTML;
                    malamRow.insertCell(1).innerHTML = dateCell.innerHTML;
                    malamRow.insertCell(2).innerHTML = tempCell.innerHTML;
                    malamRow.insertCell(3).innerHTML = humidityCell.innerHTML;
                }
            });

            // Draw temperature chart
            const tempCtx = document.getElementById('tempChart').getContext('2d');
            const tempChart = new Chart(tempCtx, {
                type: 'line',
                data: {
                    labels: labels,
                    datasets: [{
                        label: 'Temperature',
                        data: temperatureData,
                        backgroundColor: 'rgba(255, 99, 132, 0.2)',
                        borderColor: 'rgba(255, 99, 132, 1)',
                        borderWidth: 1
                    }]
                },
                options: {
                    scales: {
                        y: {
                            beginAtZero: false
                        }
                    }
                }
            });

            // Draw humidity chart
            const humidityCtx = document.getElementById('humidityChart').getContext('2d');
            const humidityChart = new Chart(humidityCtx, {
                type: 'line',
                data: {
                    labels: labels,
                    datasets: [{
                        label: 'Humidity',
                        data: humidityData,
                        backgroundColor: 'rgba(54, 162, 235, 0.2)',
                        borderColor: 'rgba(54, 162, 235, 1)',
                        borderWidth: 1
                    }]
                },
                options: {
                    scales: {
                        y: {
                            beginAtZero: false
                        }
                    }
                }
            });
        })
        .catch(error => console.error('Error reading data:', error));
}

document.addEventListener('DOMContentLoaded', function() {
    readData(); 
    setInterval(readData, 60000); 
});
