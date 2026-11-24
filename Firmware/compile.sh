#!/bin/bash

# Para o script imediatamente em caso de erro
set -e

# --- Configurações ---
MCU=atmega88p
OUTPUT_DIR="Binarios"
TARGET="RLDSP_88_RTC"
SIGNATURE_ADDR=0x1800
SOURCES="RLDSP_88_RTC.c Mod_Timer.c Mod_RTC.c Mod_Driver.c"

# --- 0. Limpeza (Clean) ---
echo "[0/5] Limpando arquivos antigos..."
rm -rf "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR"

echo "[1/5] Compilando arquivos objeto (.o)..."
for src in $SOURCES; do
    # Verifica se o arquivo existe antes de compilar
    if [ ! -f "$src" ]; then
        echo "  [ERRO] Arquivo fonte não encontrado: $src"
        exit 1
    fi

    obj="${src%.c}.o"
    echo "  -> Compilando $src..."
    # Se o avr-gcc falhar aqui, o 'set -e' interrompe o script
    avr-gcc -mmcu=$MCU -Os -c "$src" -o "$OUTPUT_DIR/$obj"
done

echo "[2/5] Realizando Linkagem (.elf e .map)..."
OBJS=$(ls $OUTPUT_DIR/*.o)
avr-gcc -mmcu=$MCU -Os -Wl,-Map,$OUTPUT_DIR/$TARGET.map \
    -Wl,--section-start=.assinatura=$SIGNATURE_ADDR \
    $OBJS -o $OUTPUT_DIR/$TARGET.elf

echo "[3/5] Gerando listagem e símbolos..."
avr-objdump -h -S $OUTPUT_DIR/$TARGET.elf > $OUTPUT_DIR/$TARGET.lss
avr-nm -n $OUTPUT_DIR/$TARGET.elf > $OUTPUT_DIR/$TARGET.sym

echo "[4/5] Extraindo .hex e .bin..."
avr-objcopy -O ihex -R .eeprom $OUTPUT_DIR/$TARGET.elf $OUTPUT_DIR/$TARGET.hex
avr-objcopy -O binary -R .eeprom $OUTPUT_DIR/$TARGET.elf $OUTPUT_DIR/$TARGET.bin

echo "[5/5] Relatório de memória:"
avr-size --format=avr --mcu=$MCU $OUTPUT_DIR/$TARGET.elf

echo "------------------------------------------------"
echo "CONCLUÍDO COM SUCESSO!"
