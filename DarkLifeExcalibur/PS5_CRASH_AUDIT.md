# Auditoría técnica del crash de carga/respawn en PS5

Fecha: 2026-07-18  
Proyecto: DarkLifeExcalibur, Unreal Engine 5.6  
Alcance: C++, configuración, metadatos legibles de Blueprints, logs, artefactos de símbolos y compilaciones Win64/PS5. No se inspeccionó ni modificó código confidencial del SDK.

## Resultado ejecutivo

La causa principal queda confirmada por símbolos: `UControllerIconManagerSubsystem::OnPostLoadMap` programaba tres callbacks en `FTSTicker` que capturaban un `UWorld*` crudo. El subsystem pertenece al GameInstance y sobrevive al cambio de mapa; el mundo capturado puede destruirse antes de que se ejecuten los callbacks de 0.25, 1.0 o 2.5 segundos. El callback pasaba después ese puntero obsoleto a `GetAllWidgetsOfClass`, lo que terminaba accediendo al UObject inválido y producía el `SIGBUS`.

La textura `FaceUp` y los mensajes `HotPlug`/`KeyPollGamepad` pertenecen al mismo sistema de UI y explican su proximidad temporal, pero no son la instrucción que falla. El backtrace identifica el reescaneo diferido del mundo, no la lectura de la textura ni el polling de teclas.

## Evidencia simbolizada

Con base `0x23028000`, el `Symbols.map` Development existente antes de la corrección resolvió:

| Offset | Símbolo |
|---|---|
| `0x00BAE0D8` | `UObjectBaseUtility::GetPathName(...TStringBuilder...)` |
| `0x00BAB685` | `UObjectBaseUtility::GetPathName(UObject const*)` |
| `0x0680D2E8` | `UEngine::GetWorldFromContextObject` |
| `0x04B0D014` | `UWidgetBlueprintLibrary::GetAllWidgetsOfClass` |
| `0x076EC3DE` | `UControllerIconManagerSubsystem::ApplyIconSetToAllWidgets` |
| `0x076F9A66` | ejecución de la lambda creada por `UControllerIconManagerSubsystem::OnPostLoadMap` |
| `0x000E83CF` | infraestructura de `FTSTicker` |

La cadena exacta es:

`FTSTicker` → lambda diferida de `OnPostLoadMap` → `ApplyIconSetToAllWidgets(LoadedWorld)` → `GetAllWidgetsOfClass` → `GetWorldFromContextObject` → `GetPathName` sobre contexto destruido → `SIGBUS`.

Código original responsable: `Source/DarkLifeExcalibur/Private/ControllerIconManagerSubsystem.cpp`, función `OnPostLoadMap`. Código corregido: líneas 374-412 y validación adicional en línea 477.

## Hallazgos por severidad

### Crítico — confirmado y corregido

- Uso-after-free lógico del `UWorld*` capturado por callbacks diferidos del ticker. Se sustituyó por `TWeakObjectPtr<UWorld>`, `CreateWeakLambda`, validación de UObject y comprobación contra el mundo actual.
- Accesos a `ShieldAttackAnimations[0..2]` antes de comprobar el índice, y validación de `ComboCounter` mientras se indexaba con `animationIndex`. Corregidos en `CPP_DarkLifeCharacter.cpp:774-842`.

### Alto — confirmado y corregido

- Dos declaraciones de `GameInstanceClass`; la última seleccionaba `UDLE_OnlineGameInstance`, que no hereda de `UCPP_GameInstance`, aunque el flujo de SaveGame/checkpoint depende de `DarkLifeGameInstance_C`. Se eliminó el override tardío. El subsystem `UDLE_EOSSubsystem` conserva la integración online sin reemplazar el GameInstance de guardado.
- Reflexión insegura de la propiedad Blueprint `Save Game`: se desreferenciaban `FProperty*` y memoria reinterpretada sin validar tipo. Se centralizó en `ResolveSaveGameObject`, usando primero la propiedad nativa y después `FObjectPropertyBase` como compatibilidad segura.
- `LoadParameters` desreferenciaba el resultado de `Cast<UCPP_GameInstance>` sin comprobarlo. Corregido con `ensureMsgf`, log y retorno seguro.

### Medio — confirmado, no modificado por no ser causa del crash actual

- El SaveGame no tiene versión propia, migración ni validación explícita de compatibilidad/corrupción.
- `CinematicActivation` persiste `TMap<ULevelSequence*, bool>` en vez de identificadores estables/soft paths. Un rename, redirect roto o asset ausente puede invalidar compatibilidad entre builds.
- `CheckPointTransform` y los transforms por nivel no se validan contra NaN/infinito antes de que los Blueprints los apliquen.
- El Blueprint `DarkLifeSaveGame` declara campos con los mismos nombres que propiedades nativas (`MissionComplete`, `MissionInfoActivation`, `CinematicActivation`, `CurrentMission`); el compilador lo advierte y existe riesgo de estado duplicado o de migración ambigua.
- La compilación de Blueprints detectó assets heredados/demo con nodos VR inexistentes y referencias a `PawnActionsComponent` ausente. No aparecen en el backtrace actual, pero deben excluirse del cook o repararse.
- La build PS5 informó que EOS usa un fallback de binarios para una versión anterior del SDK de plataforma. Debe verificarse compatibilidad con el proveedor antes de certificar.

### Bajo

- `GetRuneBySlotIndex` aceptaba sólo 0..2 aunque existía el slot 3; corregido.
- Persisten avisos de APIs deprecadas en EOS y `SuggestProjectileVelocity`.
- Hay material sensible de autenticación/configuración versionado y un token de desarrollo registrado en logs por código. No se copian valores aquí. Deben rotarse, retirarse del repositorio y evitarse en logs de Shipping.

## Correcciones e instrumentación

- `ControllerIconManagerSubsystem.cpp`: callbacks débiles, mundo débil, rechazo de mundo descargado y logs de reescaneo.
- `CPP_GameInstance.cpp/.h`: resolución segura del SaveGame, resultado del guardado y categoría `LogGameLoad`.
- `CPP_DarkLifeCharacter.cpp`: validación del GameInstance, SaveGame, PlayerController e índices; logs de inicio/fin de restauración.
- `CPP_ItemContainer.cpp`: rango correcto para cuatro slots.
- `Config/DefaultEngine.ini`: un único GameInstance coherente con los Blueprints de carga.
- `GameLoadLog.h` y `DarkLifeExcalibur.cpp`: declaración/definición de `LogGameLoad`.

Los logs incluyen mundo, objeto, slot cuando aplica, hilo y resultado. Las etapas que sólo existen dentro de Blueprints binarios (solicitud de OpenLevel, streaming completo, Pawn poseído, checkpoint validado, cinemática iniciada/control devuelto) deberán conectarse manualmente a `LogGameLoad` o convertirse en una API C++ común en una iteración separada.

## Verificación ejecutada

- UnrealHeaderTool Win64 y PS5: correcto.
- Target `DarkLifeExcalibur Win64 Development`: correcto.
- Target `DarkLifeExcalibur PS5 Development`, incluido enlace y generación de símbolos: correcto después de todos los cambios.
- Target Editor: compilación C++ correcta; el enlace no pudo reemplazar el DLL porque Unreal Editor estaba abierto. No se cerró la sesión del usuario.
- `CompileAllBlueprints`: terminó con código 26 por 54 errores y 142 warnings preexistentes. Los flujos centrales `SavePoint`, `SavePoint_Start`, `BP_Checkpoint` y `BP_CinematicSequence` fueron recorridos; se observaron las advertencias de campos duplicados del SaveGame. Los fallos fatales mostrados corresponden principalmente a assets demo/VR obsoletos y una misión con firma de delegado desactualizada.
- 19.903 paquetes `.uasset/.umap`: ninguno vacío o menor de 64 bytes.
- Búsqueda final: ya no quedan las lambdas `[this, LoadedWorld]`, los `reinterpret_cast` del SaveGame ni los accesos de animación que indexaban antes de validar.

## Riesgos no verificados

- No se ejecutó el juego corregido en el Dev Kit, por lo que falta confirmar la desaparición del crash bajo carga/respawn real.
- No se proporcionó el crash dump original completo ni el ejecutable exacto/identificador de build; la correlación se hizo con direcciones, base y el mapa Development presente, que produjo una coincidencia completa del flujo.
- No se pudo reconstruir visualmente todo el bytecode/orden de ejecución de Blueprints binarios. Abrir manualmente: `DarkLifeGameInstance`, `DarkLifeSaveGame`, `BP_Checkpoint`, `SavePoint`, `SavePoint_Start`, `BP_CinematicSequence`, `GreystonePlayerCharacter2_old`, `DarkLifeGameMode` y los PlayerControllers asociados.
- Existe un `.sav` local, pero no se mutó ni se probó como guardado antiguo/corrupto.

## Matriz mínima para la siguiente build

1. Nueva partida sin `.sav` previo.
2. Guardar y cargar un guardado recién creado.
3. Cargar una copia de guardado de versión anterior.
4. Probar guardado truncado/corrupto y comprobar rechazo sin crash.
5. Reiniciar checkpoint y repetir muerte/respawn al menos 20 veces.
6. Ejecutar transición de nivel durante cinemática y omitir/finalizar la secuencia.
7. Conectar/desconectar control durante carga y durante los 3 segundos posteriores a `PostLoadMap`.
8. Alternar rápidamente dos mapas para forzar que los callbacks viejos encuentren el mundo inválido; debe aparecer `Deferred UI rescan skipped`, nunca un crash.
9. Repetir carga/respawn 20 veces en Development y Test/Shipping.
10. Comparar PC empaquetado y PS5, verificando orden de `LogGameLoad`.

## Archivos necesarios si se requiere simbolización adicional

- Dump/reporte completo del crash del Dev Kit y log completo de la misma ejecución.
- `eboot.bin` o ELF/self no transformado correspondiente exactamente a esa build, según el flujo autorizado del equipo.
- `Symbols.map` y archivos de símbolos generados por esa misma compilación.
- Identificador/changelist, configuración (Development/Test/Shipping), fecha y base de carga observada.
- Si el crash cambia después del parche: nuevas direcciones absolutas y módulos cargados con sus bases.

No deben compartirse claves, credenciales, archivos de licencia ni contenido confidencial del SDK.
