# Compilar Aya Player no Windows com menos uso de disco

O preset `x64-windows-player` configura apenas o Player. Ele desliga o CEF,
dispensa QtitanRibbon e Studio, usa o vcpkg dinâmico `x64-windows` e não
instala CGAL nem Boost.Test. O Player ainda usa Qt WebEngine, portanto a
instalação do Qt e das bibliotecas C++ continua ocupando vários GB.

## Preparação

1. Instale o Visual Studio 2022 com **Desktop development with C++**,
   **C++ Clang tools for Windows**, CMake, Ninja e um Windows SDK.
2. Instale o Qt 6 para MSVC 2022 x64, incluindo **Qt WebEngine**. Anote a
   pasta, por exemplo `C:\Qt\6.8.3\msvc2022_64`.
3. Abra **Developer PowerShell for VS 2022** em modo x64. Confirme que
   `cmake`, `ninja`, `clang-cl` e `git` funcionam nesse terminal.
4. Transfira `aya-player-source.bundle` da VPS para o PC. Esse bundle contém
   o código versionado; não contém builds, vcpkg instalado nem cache.

## Fonte e vcpkg

No Developer PowerShell, ajuste `C:\Aya` se quiser outra pasta:

```powershell
cd C:\
git clone .\caminho\para\aya-player-source.bundle Aya
cd C:\Aya
git clone --filter=blob:none --no-checkout https://github.com/microsoft/vcpkg.git vcpkg
git -C vcpkg checkout 29ff5b8131d0c6c8fcb8fbaef35992f0d507cd7c
.\vcpkg\bootstrap-vcpkg.bat
```

O commit do vcpkg acima coincide com `builtin-baseline` dos manifestos.
Se o bundle estiver em Downloads, substitua o caminho no `git clone`.
Depois do clone, o arquivo `.bundle` pode ser apagado para liberar espaço.

## Configurar e compilar

Execute no mesmo Developer PowerShell:

```powershell
cd C:\Aya
$env:VCPKG_BINARY_SOURCES = "clear"
cmake --preset x64-windows-player -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/msvc2022_64"
cmake --build --preset x64-windows-player --target Player --parallel 2
```

Troque `C:/Qt/6.8.3/msvc2022_64` pela pasta real. O primeiro `cmake` instala
as dependências vcpkg e pode demorar. O segundo cria
`C:\Aya\build\x64-windows-player\bin\Aya.Player.exe` e copia as DLLs de Qt
via `windeployqt` e `fmod.dll`. O cache binário foi desligado para evitar
outra cópia das bibliotecas; futuras recompilações podem demorar mais.

Para um teste local com conteúdo do repositório:

```powershell
cd C:\Aya
.\build\x64-windows-player\bin\Aya.Player.exe --play --content "C:/Aya/client/resources/content" --console
```

O Player busca `ClientSettings` no site `clipaai.tech`. Entrar em um jogo
real requer o ticket e a URL de join gerados pelo site ou pelo launcher.
`appsettings.ini` não faz parte do Aya Player; a configuração local usa
`QSettings`, e os recursos ficam em `client/resources/content`.

Para copiar a build a outra pasta, copie o diretório `bin` e também
`client/resources/content` como `bin/content`. Faça isso só depois de
compilar para não duplicar os arquivos durante o build.

Após um build bem-sucedido, é possível remover `vcpkg/buildtrees` e
`vcpkg/packages` para recuperar espaço. Mantenha `build/x64-windows-player`
e `vcpkg_installed` para recompilar sem reinstalar as dependências.
