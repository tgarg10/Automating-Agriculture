#include <pgmspace.h>

#define SECRET
#define THINGNAME "esp32-cam1"

const char WIFI_SSID[] = "IT2017";
const char WIFI_PASSWORD[] = "cmkmsp006";
const char AWS_IOT_ENDPOINT[] = "a3611c7i1de58-ats.iot.ap-southeast-1.amazonaws.com";

// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

// Device Certificate
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUGrQfOzyGVY1SD0UdO26wCDbF0m8wDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIxMTEwMjE0MzIw
M1oXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAPUgCHlYNmUktk6p+Ku6
283JN35WdisCpAtyvzo9FPDmg4XPwkyY5xJ436khWyEpm/UrV3GnWQAAyN7fy+wb
6X2Qt3G4gB3r1Uixe+nUkdSrbBQJC84IIprTHpTQ4VcfvKCb/a7HSiPjFISrtXQx
DvhJMW98/wL15P10MeJM9ST8RV+CQzCtBCjPdWltomCTm1d7CkrH1pTngpe/eLFr
s5aB8z9ZAaOy018MGQ0yHfaN6WLe07vk2y3DavtS1kAOrLbCO7VzVqsc3MCYA2VX
o1i85Gdwqt1vQFa6PTQlXu0JEvenSiOjVoX1LsZ8qNOv30yi/Cc357IvyHugqoaK
M78CAwEAAaNgMF4wHwYDVR0jBBgwFoAUKTR9UKlpFdoJwTIm2nVi8DiAoiYwHQYD
VR0OBBYEFG7NWEB0gUt0ukclisuBmhZ1s/vgMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAmaTjvfmzb8Sf6Acox1yEr2fZz
8r7e4FVxLxi18n6ZoTEA3PpYY4W1WuAT4jiBqYpuMhzCaq14jeDyzp9dbOc67UzR
I0g9aYYUw3E/Y1gw11Gpi91PRbSkgZVEPTWRCoGnKTPJAcW6ibKUDsVl4AyFlbaU
CmyhuwGcwqGyKFr+9oOv6upWLnrUl3woiaQ0212UPRQuivdCv2WKJ1/pCf0EOnF6
E7jCQ/yJW2vN1RzAENro1JWkRxTvYYHZJak4hJ+3mBtjIx0KUX/1D4CDJEndFiCn
9YLREIe0UMrptUnMS7uHe1KS9lXovsdaQnovjbbRODNv/d543L1H/YN7xeQP
-----END CERTIFICATE-----
)KEY";

// Device Private Key
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEA9SAIeVg2ZSS2Tqn4q7rbzck3flZ2KwKkC3K/Oj0U8OaDhc/C
TJjnEnjfqSFbISmb9StXcadZAADI3t/L7BvpfZC3cbiAHevVSLF76dSR1KtsFAkL
zggimtMelNDhVx+8oJv9rsdKI+MUhKu1dDEO+Ekxb3z/AvXk/XQx4kz1JPxFX4JD
MK0EKM91aW2iYJObV3sKSsfWlOeCl794sWuzloHzP1kBo7LTXwwZDTId9o3pYt7T
u+TbLcNq+1LWQA6stsI7tXNWqxzcwJgDZVejWLzkZ3Cq3W9AVro9NCVe7QkS96dK
I6NWhfUuxnyo06/fTKL8Jzfnsi/Ie6CqhoozvwIDAQABAoIBABaTgLRMX5YpL3FH
TlPivePSKH5GUFPdxmYXh21BuvAFCxAD9o0/d+jHoTHelJoVTvvhpggdtaC+ccG1
Mv7oAzVBYuPibkpTMi2jyxnaLiQ8NZQS0TeEoEtKLoLeSiLQFXtlYpP6XuW3QGiH
uAqfihVsCFWQWZxYMWL2lLGXDOz+8O4MjzXAfJvPZRLSENxUQ5xw802IhvSOU4OL
ppbcGCpZzjifjIYO0rqFIpVJtZEjC/3rZrtEIGZ1Ae3009FTzc8huUA/hqTSIXRU
OmYAzzBpLoVLD0NU0FEsBvSioOTJ/B1KRfZGXyKr0LZUYb8n2xS6AVBWRbDEBYO9
5OCBWQECgYEA/wXsMi43uykBpPxYkvtFFoWY4uaFRDir93DKYvmYec9DZFKQZLmG
Bd+i8e6S82dGN3gU5R+7dAsiEh2WKSOlSSYwCLJvFUk7752p3W1wgxhvETaoR7bP
+Jw+2/zxuXw1HXQSiI3RrV3vlBd2HnaUqylPUaTFQZ2kDK5Ysg9B2WECgYEA9hBn
h37+QuEY7lwnZU5bPrmZjhfs4NyaNGHcZS1HIxx+fdtnhnP1H+PZPm2qaWDwngb5
6kFytI4gvKngkQcNTmddwvXARfK5z4NjmWeQuWRsaDT0+zlzL7lHoadxKfYMwMiy
TopEyt9JfWnDL8p2Nc9N+ym9NU378cKeS6lngR8CgYATUvmQ29uxPOw4+j4j9RJJ
IJpzJxCWGM0/FLbhRRR+tmve4Dq8xyoWq9mHB5LvCrZRYbIDoqHrqyRLxx4nw+G0
Z0ZgglTUghYelA3ZD3/lDybrYP7jYEDmD8DbnpEagwTrVG5LFqP6xRFwYU68BDRD
u/+cqiWgUTerFlL2Ox28QQKBgQCfJjUP+h9R+m05nfkNGQUDKOAce3UGayiBwNnf
xs0/9GoL1SC8YbuliKU2L5AM4FpAm5qA3RvnlBWwcN+CbeE5WLnYI0Lc+wUbJowA
6G8+uak0LSxWYO4Ta+7cq4QR88aD2ksZaWzdGlZZ+1H3Uyfp3c8+J9zs4SeXUp03
qrX7WQKBgQDE1SQUS7v1cInomL7FC1ZtfkoekmTOaR9F7L8U9E3kSEqvw3C90LrD
zFMW6pf2BRXj1C2IEDUjWOOZXfvnUXNPugYMNIlo2ozVgS6+yxWsoPoIHIkQeJL+
eNLNwwGB3BfbBfkehSgcy1oq4rtBDvdvP65cZR68D9p5vOEGuPfXzQ==
-----END RSA PRIVATE KEY-----
)KEY";

/////////////////////////////////////////

/*
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

// Device Certificate
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUQ4LaHztqiaIemxDUaS/mzxGqZuUwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIxMTAxNDE1NTE1
MVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAN17bcVLQ6dUlaDJL5Ji
UfUs+x0lZB6ZSF8icbGoz8bfyC8sIzya/r1jcwauuOb2xju+XBvyDZQB1ksGQaUT
y3xrU1eqX2niJGhMmLfZSAlhB2j4Mdw3HAiqiF1cR+MfUhG4P8J6kra7z+NK2jUr
tddyHCRzODe6KCrLfAMlsn1e8CoQKPwJ5888mccaBmSV+hjv+nzle0olRF3HAFuU
f4ih9UiTTZV+sUwzFxyPSUflgBsmTRNVDeDK6klew/haPPfN+oYyE0Xw48kcPIhn
TG+ZbamSXi9Todp5+4+bYLs518wxUAY+awryFPZ+T+JHvEw8wfAS8Pp4V1SSCmtr
REECAwEAAaNgMF4wHwYDVR0jBBgwFoAUhI4v6CAKA01RzeWs0pumJzCDfx8wHQYD
VR0OBBYEFKxLJrQsBoc9AqnF+U8026+bJbcDMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCcClIMS9fVuKWlheWHy/Yv1maH
pwCK3chzv+4yIXvlGJ8rRvw71SMP0do7w3YtVSDG5oPPc8dZ/KJ+44tp4Uo50mn9
16RsrYa4w/il+pzc/vw+IxP5nb82iq3Rh6/vZH0DHNHVkK4BfPC0zz1AafS4shYC
IMOdSP1jPh9NTrDIpYQVV07nxxJqYv2Ah85ESDohI02WzHmcRrws+A56kZFRu+mf
4PYILYCiVkGYHCMlhcU/7evPz2xj7OSy5mA/R9RoRFqMLCAORpSvnfLppm3H1Qp8
/wNtdTIn9cVq9HJqvX1e2QcEsqvz/MxmxZFO71ipI8qUio0chZmTCBAB7c+9
-----END CERTIFICATE-----
)KEY";

// Device Private Key
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEA3XttxUtDp1SVoMkvkmJR9Sz7HSVkHplIXyJxsajPxt/ILywj
PJr+vWNzBq645vbGO75cG/INlAHWSwZBpRPLfGtTV6pfaeIkaEyYt9lICWEHaPgx
3DccCKqIXVxH4x9SEbg/wnqStrvP40raNSu113IcJHM4N7ooKst8AyWyfV7wKhAo
/AnnzzyZxxoGZJX6GO/6fOV7SiVEXccAW5R/iKH1SJNNlX6xTDMXHI9JR+WAGyZN
E1UN4MrqSV7D+Fo89836hjITRfDjyRw8iGdMb5ltqZJeL1Oh2nn7j5tguznXzDFQ
Bj5rCvIU9n5P4ke8TDzB8BLw+nhXVJIKa2tEQQIDAQABAoIBAQDUKHncNd6WmUXD
gTSaqqgLjYz2PG0NipNy3hG+txBhVtfxp/wQ///o9yymSBRC2jzF2fFtq44qKyBV
OacENFQF2OE65q8DRGbPqh/YbMC+iz2H5cgNRJUMEB8UH2OVQtlgs/gPmU6U0I+x
EHC54icMO/iNSfmeGCqF4+nlMeUqhVAo5PTevwsDsyF4U7O29swWiGdi2lrewXPM
/pXS+3neq5rdMQDTNEa0Wy+uBVTUfoa5sDMl9lct80xVBZdLeLxnJMfkwgPLWHmm
uQMieBA+yePYptu9+n2SRqtmaTaabK0s0B3N1cgRNYANnT5Qp8w04D130lonvVEX
BrcNHXwJAoGBAPlSoGhtBj3LBxWibzd6NQMoea300Ve9xTkTfh5On5xwRNuy66Cs
9iRkKrB9uRRlQQSTCsCabntEXM6r5I9gpmbrWp1V8yFmoCElaqRyf2kUAknMpBiL
xQFG1yTx3jq0cN6q28KYXbPGLBXYvBg6lDhdtWFz+Kja737hi4eNB8GrAoGBAONp
7NI3jX0Zj2VIwGNdAWAPp96pIobU4uquiITO2G+7W6JBRKxLuhre132ny7cAoCWa
ItpTfhGaRmmuMFJKmSBr0Z+mvaIxilXtScv5chNmUIqjpF8U0qfuFxl47fkEPDkr
yNjvYlwTUaI55hYo8cwO57xAsnWfo366EAU9Fz3DAoGBAJXnpCKlSpdktwG/BHeq
tq0LbP22sRe8NhS3L6ExTK5WolAmJ54X4HbI8xHixaEJsCn77v8W206qL9Ljbyz1
w5cl8vmeb+ypM6Y6RQBTB2A6c5XDwPYxM2YtHbZM5n0NeL+oozFBQzWP9Xm9Mnqu
3ed2EFRj0iJUYKM9IRODLCK5AoGBAJbMrM+RKu+jvWTKY1kAWWMbvpN10IFcNcSt
S7XVe8xLjy3+3czqwlQDJR4b4IhPj+KfXTEtYFHH3Z1CdqR69yN6m5ykcudUtojY
OrFZjf2zQQFOFjc+lZnp1cN8X0FbP5whnqPLyYuI+/75KcvUBtLrp1s8A5rf6wij
1O/EXN8FAoGAc7+ut2pqhQUIvnXoyCP8K6zC/JF6KGP/7mJXxRfU4WeUCgh5D0cq
dV+EeYgd8hPpSVgiqvbXmF/nxHYgNj4ORN6uHbP4dONlw3JYx/gRj6i8KHtvkwLw
9d9jMt7IgH7EDigw6omifrq6CyqF2G1xhyZSirozVEL72uS467OPe7g=
-----END RSA PRIVATE KEY-----
)KEY";

*/
