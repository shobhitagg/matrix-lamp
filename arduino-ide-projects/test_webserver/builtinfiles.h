/**
 * @file builtinfiles.h
 * @brief HTML content for the RGB Controller
 */

static const char indexContent[] PROGMEM = R"==(
<!doctype html>
<html lang='en'>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32-C3 RGB Control</title>
  <style>
    body { font-family: sans-serif; text-align: center; padding: 20px; }
    input[type="color"] { border: none; width: 100px; height: 100px; cursor: pointer; }
    h1 { color: #333; }
  </style>
</head>
<body>
  <h1>RGB Light Control</h1>
  <p>Select a color below:</p>
  
  <input type="color" id="picker" value="#000000">
  
  <p id="status">Current Color: #000000</p>

  <script>
    let picker = document.getElementById('picker');
    let status = document.getElementById('status');

    // Debounce function to prevent flooding the ESP with requests
    let timeout;
    picker.addEventListener("input", function(event) {
        clearTimeout(timeout);
        status.innerText = "Current Color: " + event.target.value;
        timeout = setTimeout(() => {
            updateColor(event.target.value);
        }, 100); // 100ms delay
    }, false);

    function updateColor(hex) {
        // Convert hex to RGB
        let r = parseInt(hex.substring(1, 3), 16);
        let g = parseInt(hex.substring(3, 5), 16);
        let b = parseInt(hex.substring(5, 7), 16);

        // Send to ESP32
        fetch('/api/set_rgb?r=' + r + '&g=' + g + '&b=' + b)
          .then(response => console.log("Color updated"))
          .catch(error => console.error('Error:', error));
    }
  </script>
</body>
</html>
)==";