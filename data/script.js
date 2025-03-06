document.getElementById('imageInput').addEventListener('change', function(event) {
    const file = event.target.files[0];
    if (file) {
        const reader = new FileReader();
        reader.onload = function(e) {
            const img = new Image();
            img.onload = function() {
                const canvas = document.getElementById('imageCanvas');
                const ctx = canvas.getContext('2d');

                // Ustawienie rozmiaru canvas na 128x160
                canvas.width = 128;
                canvas.height = 160;

                // Rysowanie obrazu na canvas z przeskalowaniem
                ctx.drawImage(img, 0, 0, 128, 160);

                // Konwersja do formatu baseline JPEG
                const dataUrl = canvas.toDataURL('image/jpeg', 1.0);

                // Tworzenie linku do pobrania
                const downloadLink = document.getElementById('downloadLink');
                downloadLink.href = dataUrl;
                downloadLink.download = 'przetworzone_zdjecie.jpg';
                downloadLink.style.display = 'block';

                // Wysyłanie zdjęcia na serwer ESP32
                fetch('/upload', {
                    method: 'POST',
                    body: JSON.stringify({ image: dataUrl }),
                    headers: { 'Content-Type': 'application/json' }
                })
                .then(response => response.text())
                .then(data => console.log(data))
                .catch(error => console.error('Błąd:', error));
            };
            img.src = e.target.result;
        };
        reader.readAsDataURL(file);
    }
});