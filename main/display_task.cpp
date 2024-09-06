#include <driver/gptimer.h>
#include "display_task.h"
#include "Adafruit_Protomatter/src/core.h"
#include "Adafruit_Protomatter.h"
#include "Fonts/FreeSans9pt7b.h"

static const char *TAG = "display_task";

extern time_t sntp_now;
extern struct tm sntp_timeinfo;

uint8_t rgbPins[] = {25, 26, 27, 14, 15, 13};
uint8_t addrPins[] = {23, 22, 5, 2};
uint8_t clockPin = 21; // Must be on same port as rgbPins
uint8_t latchPin = 4;
uint8_t oePin = 15;

Adafruit_Protomatter matrix(
        64,          // Width of matrix (or matrix chain) in pixels
        4,           // Bit depth, 1-6
        1, rgbPins,  // # of matrix chains, array of 6 RGB pins for each
        4, addrPins, // # of address pins (height is inferred), array of pins
        clockPin, latchPin, oePin, // Other matrix control pins
        true);      // No double-buffering here (see "doublebuffer" example)


void test_display() {

    ProtomatterStatus status = matrix.begin();

    // Make four color bars (red, green, blue, white) with brightness ramp:
    /*for (int x = 0; x < matrix.width(); x++) {
        uint8_t level = x * 256 / matrix.width(); // 0-255 brightness
        matrix.drawPixel(x, matrix.height() - 4, matrix.color565(level, 0, 0));
        matrix.drawPixel(x, matrix.height() - 3, matrix.color565(0, level, 0));
        matrix.drawPixel(x, matrix.height() - 2, matrix.color565(0, 0, level));
        matrix.drawPixel(x, matrix.height() - 1,
                         matrix.color565(level, level, level));
    }
    // You'll notice the ramp looks smoother as bit depth increases
    // (second argument to the matrix constructor call above setup()).

    // Simple shapes and text, showing GFX library calls:
    matrix.drawCircle(12, 10, 9, matrix.color565(255, 0, 0));
    matrix.drawRect(14, 6, 17, 17, matrix.color565(0, 255, 0));
    matrix.drawTriangle(32, 9, 41, 27, 23, 27, matrix.color565(0, 0, 255));

    matrix.setFont(&FreeSans9pt7b);

    matrix.write("TEST");
    matrix.print("ABC");

    //matrix.println("ADAFRUIT"); // Default text color is white
    if (matrix.height() > 32) {
        matrix.setCursor(0, 32);
        //matrix.println("64 pixel"); // Default text color is white
        //matrix.println("matrix"); // Default text color is white
    }

    // AFTER DRAWING, A show() CALL IS REQUIRED TO UPDATE THE MATRIX!*/

    matrix.show(); // Copy data to matrix buffers
}

void display_task(void *parm) {
    ESP_LOGI(TAG, "display_task start");

    while (true) {

        matrix.setCursor(0, 0);
        matrix.fillScreen(0);

        //matrix.setFont(&FreeSans9pt7b);

        time(&sntp_now);

        char strftime_buf[64];
        localtime_r(&sntp_now, &sntp_timeinfo);

        if (sntp_timeinfo.tm_year < (2016 - 1900)) {
            matrix.write("syncing...");
        } else {
            strftime(strftime_buf, sizeof(strftime_buf), "%c", &sntp_timeinfo);
            matrix.write(strftime_buf);
        }

        matrix.show(); // Copy data to matrix buffers

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}