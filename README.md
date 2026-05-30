# Solano-Post1-U10 - Benchmark de Jerarquía de Caché

## Objetivo
Medir empíricamente la latencia de acceso a memoria para arrays de distintos
tamaños, observando el efecto de la jerarquía de caché (L1, L2, L3, RAM)
mediante acceso secuencial y aleatorio en C.

## Entorno
- SO: Ubuntu 24.04 (WSL2 en Windows 11)
- Compilador: GCC 13.3.0
- Compilación: `gcc -O0 -o cache_bench cache_bench.c`

## Caché del CPU (Checkpoint 1)
| Nivel | Tamaño |
|-------|--------|
| L1d   | 48 KB  |
| L1i   | 32 KB  |
| L2    | 1280 KB (1.25 MB) |
| L3    | 12288 KB (12 MB)  |

## Ejecución
```bash
mkdir -p ~/u10post1 && cd ~/u10post1
gcc -O0 -o cache_bench cache_bench.c && ./cache_bench
```

## Acceso Secuencial (Checkpoint 2)
| Array size (KB) | ns/byte |
|-----------------|---------|
| 4               | 0.591   |
| 8               | 0.565   |
| 16              | 0.571   |
| 32              | 0.660   |
| 64              | 0.667   |
| 128             | 0.653   |
| 256             | 0.557   |
| 512             | 0.575   |
| 1024            | 0.547   |
| 4096            | 0.537   |
| 16384           | 0.537   |
| 65536           | 0.523   |


## Secuencial vs Aleatorio (Checkpoint 3)
| Size (KB) | SEQ ns/byte | RAND ns/elem |
|-----------|-------------|--------------|
| 4         | 0.740       | 1.495        |
| 8         | 0.724       | 1.996        |
| 16        | 0.675       | 1.350        |
| 32        | 0.588       | 1.704        |
| 64        | 0.664       | 1.366        |
| 128       | 0.646       | 1.304        |
| 256       | 0.627       | 1.190        |
| 512       | 0.602       | 1.557        |
| 1024      | 0.599       | 1.645        |
| 4096      | 0.627       | 3.042        |
| 16384     | 0.763       | 11.824       |
| 65536     | 0.776       | 14.943       |

## Análisis de Resultados

### Acceso Secuencial
Los valores SEQ se mantienen estables (~0.5–0.67 ns/byte) en todos los
tamaños. Esto se debe a que el acceso secuencial aprovecha la localidad
espacial: el prefetcher del CPU carga líneas de caché anticipadamente,
ocultando la latencia real de cada nivel.

### Acceso Aleatorio
El acceso aleatorio destruye la localidad espacial, exponiendo los saltos
reales entre niveles:

- **4–1024 KB** (~1.3–2.0 ns): el array cabe dentro del L3 (12 MB),
  los accesos se resuelven mayormente en caché.
- **4096 KB** (3.0 ns): el array comienza a superar la capacidad del L3,
  aparecen cache misses frecuentes.
- **16384 KB → 65536 KB** (11.8–14.9 ns): el array supera completamente
  el L3, cada acceso aleatorio genera un cache miss que va a RAM,
  incrementando la latencia ~8x respecto a los arrays pequeños.

### Cache Miss vs TLB Miss
Para arrays mayores a ~1–2 MB, el acceso aleatorio también genera
**TLB misses**: el TLB no puede contener todas las entradas de página
necesarias, añadiendo una penalización adicional a la ya costosa búsqueda
en RAM. Esto explica por qué la latencia sigue subiendo de 16 MB a 64 MB
en lugar de estabilizarse.
