#include <Arduino.h>

// Definir los pines para los LEDs
#define LED1_PIN 23  // Pin para LED1
#define LED2_PIN 19  // Pin para LED2
#define LED3_PIN 18  // Pin para LED3

// Definir el pin para el potenciómetro (canal ADC)
#define POT_PIN 25  // Pin para el potenciómetro (canal ADC)

// Declaración de las funciones (prototipos)
void mostrarMenu();
void leerADC();
void controlarLEDs();
void solicitarLED();
void solicitarTiempo();
void encenderLED(int led, int tiempo);

// Variables para almacenar los valores leídos
int ledNumber = 0;
int ledTime = 0;
int adcValue = 0;
bool esperandoLED = false;
bool esperandoTiempo = false;

// Configurar el UART y los pines
void setup() {
  // Configurar los pines de los LEDs como salidas
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);

  // Iniciar la comunicación serial a 9600 baudios
  Serial.begin(9600);

  // Mostrar el menú inicial
  mostrarMenu();
}

// Función principal de bucle
void loop() {
  // Si hay datos disponibles en el monitor serial
  if (Serial.available() > 0) {
    // Leer la entrada del usuario
    String input = Serial.readStringUntil('\n');  // Leer hasta nueva línea

    // Si estamos esperando la selección del LED
    if (esperandoLED) {
      ledNumber = input.toInt();
      // Validar si el número de LED es válido (1, 2 o 3)
      if (ledNumber < 1 || ledNumber > 3) {
        Serial.println("Número de LED inválido. Ingrese 1, 2 o 3.");
        solicitarLED();  // Volver a pedir el número de LED
      } else {
        esperandoLED = false;  // Ya no estamos esperando el LED
        solicitarTiempo();     // Ahora pedimos el tiempo
      }
    }
    // Si estamos esperando el tiempo
    else if (esperandoTiempo) {
      ledTime = input.toInt();
      // Validar si el tiempo es válido (mayor que 0)
      if (ledTime <= 0) {
        Serial.println("Tiempo inválido. Ingrese un tiempo en milisegundos.");
        solicitarTiempo();  // Volver a pedir el tiempo
      } else {
        esperandoTiempo = false;  // Ya no estamos esperando el tiempo
        // Llamar a la función para encender el LED
        encenderLED(ledNumber, ledTime);
        // Volver a mostrar el menú
        mostrarMenu();
      }
    } else {
      // Evaluar la opción ingresada por el usuario
      char opcion = input[0];  // Tomamos el primer carácter

      switch (opcion) {
        case '1':
          leerADC();  // Si el usuario ingresa '1', leer el ADC
          break;
        case '2':
          controlarLEDs();  // Si el usuario ingresa '2', controlar los LEDs
          break;
        default:
          // Si el usuario ingresa una opción no válida
          Serial.println("Opción inválida, por favor seleccione 1 o 2.");
          mostrarMenu();  // Volver a mostrar el menú
          break;
      }
    }
  }
}

// Función para mostrar el menú en la terminal serial
void mostrarMenu() {
  Serial.println("----- Menu -----");
  Serial.println("1. Lectura ADC");
  Serial.println("2. Controlar LEDs");
  Serial.print("Ingrese una opción: ");
}

// Función para leer el valor del ADC (potenciómetro)
void leerADC() {
  // Leer el valor del canal ADC (potenciómetro)
  adcValue = analogRead(POT_PIN);

  // Convertir el valor ADC en voltaje (supongamos un rango de 0 a 3.3V)
  float voltage = adcValue * (3.3 / 4095.0);

  // Mostrar el valor de voltaje en la terminal serial
  Serial.print("Voltaje: ");
  Serial.print(voltage);
  Serial.println(" V");

  // Volver a mostrar el menú
  mostrarMenu();
}

// Función para controlar los LEDs
void controlarLEDs() {
  // Iniciar el proceso de solicitud de LED
  solicitarLED();
}

// Función para solicitar al usuario cuál LED encender
void solicitarLED() {
  Serial.println("Ingrese el número del LED a encender (1, 2 o 3):");
  esperandoLED = true;  // Esperamos que el usuario ingrese el número del LED
}

// Función para solicitar al usuario el tiempo de encendido
void solicitarTiempo() {
  Serial.println("Ingrese el tiempo en milisegundos para el LED:");
  esperandoTiempo = true;  // Esperamos que el usuario ingrese el tiempo
}

// Función para encender el LED por un tiempo determinado
void encenderLED(int led, int tiempo) {
  // Determinar qué LED encender según el parámetro ingresado
  int pin;
  switch (led) {
    case 1:
      pin = LED1_PIN;
      break;
    case 2:
      pin = LED2_PIN;
      break;
    case 3:
      pin = LED3_PIN;
      break;
  }

  // Encender el LED
  digitalWrite(pin, HIGH);
  Serial.print("Encendiendo LED");
  Serial.print(led);
  Serial.print(" por ");
  Serial.print(tiempo);
  Serial.println(" ms");

  // Mantener el LED encendido por el tiempo especificado
  vTaskDelay(tiempo / portTICK_PERIOD_MS);

  // Apagar el LED
  digitalWrite(pin, LOW);
  Serial.print("LED");
  Serial.print(led);
  Serial.println(" apagado");
}
