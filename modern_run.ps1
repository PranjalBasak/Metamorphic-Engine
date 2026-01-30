param (
    [Parameter(Mandatory = $true)]
    [int]$Chunk
)

$Root        = Split-Path -Parent $MyInvocation.MyCommand.Path
$BinDir      = Join-Path $Root "bin"
$ChunkDir    = Join-Path $BinDir "chunk_$Chunk"
$OutputRoot  = Join-Path $Root "output"
$OutputDir   = Join-Path $OutputRoot "chunk_$Chunk"
$Metamorphic = Join-Path $BinDir "metamorphic.exe"

# ===============================
# Validation
# ===============================
if (!(Test-Path $Metamorphic)) {
    Write-Error "metamorphic.exe not found in /bin"
    exit 1
}

if (!(Test-Path $ChunkDir)) {
    Write-Host "Chunk Folder Not Available."
    Write-Host "Please create a chunk folder chunk_$Chunk in directory /bin"
    exit 1
}

# ===============================
# Prepare output directory
# ===============================
if (Test-Path $OutputDir) {
    Write-Host "[INFO] Cleaning existing output directory"
    Remove-Item $OutputDir -Recurse -Force
}

New-Item -ItemType Directory -Path $OutputDir | Out-Null

# ===============================
# Generate mutated executables
# ===============================
$InputExecutables = Get-ChildItem $ChunkDir -Filter *.exe

if ($InputExecutables.Count -eq 0) {
    Write-Warning "No executables found in $ChunkDir"
    exit 0
}

foreach ($exe in $InputExecutables) {
    $OutExe = Join-Path $OutputDir ("o_" + $exe.Name)
    Write-Host "[INFO] Mutating $($exe.Name)"
    & $Metamorphic --input $exe.FullName --output $OutExe
}

# ===============================
# Interactive menu loop
# ===============================
function Show-Menu {
    Clear-Host
    Write-Host "=== Output: chunk_$Chunk ===`n"

    $items = @()
    $index = 1

    Get-ChildItem $OutputDir | ForEach-Object {
        $items += [PSCustomObject]@{
            Index = $index
            File  = $_
        }
        Write-Host "$index) $($_.Name)"
        $index++
    }

    Write-Host "`nA) Run all executables"
    Write-Host "Q) Quit"
    return $items
}

while ($true) {
    $MenuItems = Show-Menu
    $Choice = Read-Host "`nSelect option"

    if ($Choice -eq 'Q') {
        break
    }

    if ($Choice -eq 'A') {
        $Executables = Get-ChildItem $OutputDir -Filter o_*.exe
        foreach ($exe in $Executables) {
            $Log = Join-Path $OutputDir ("te_" + $exe.Name + ".txt")
            Write-Host "[RUN] $($exe.Name)"
            & $exe.FullName *>&1 | Tee-Object -FilePath $Log
            Write-Host
        }
        Read-Host "Press Enter to continue"
        continue
    }

    if ($Choice -match '^\d+$') {
        $Selected = $MenuItems | Where-Object { $_.Index -eq [int]$Choice }

        if (!$Selected) {
            Write-Host "Invalid selection"
            Start-Sleep 1
            continue
        }

        $File = $Selected.File

        if ($File.Extension -eq ".exe") {
            $Log = Join-Path $OutputDir ("te_" + $File.Name + ".txt")
            Write-Host "[RUN] $($File.Name)"
            & $File.FullName *>&1 | Tee-Object -FilePath $Log
        }
        elseif ($File.Name -like "te_*.txt") {
            Write-Host "`n=== Terminal Output: $($File.Name) ===`n"
            Get-Content $File.FullName
        }
        else {
            Write-Host "Unsupported file type"
        }

        Write-Host
        Read-Host "Press Enter to continue"
        continue
    }

    Write-Host "Invalid option"
    Start-Sleep 1
}
