param(
  [string]$Readme = "README.md",
  [string]$Path   = ".",
  [string[]]$Exclude = @(".git", "build", "node_modules", ".vs", ".vscode"),
  [switch]$Ascii
)

function Get-TreeLines {
  param(
    [string]$BasePath,
    [string]$Prefix = ""
  )

  if ($Ascii) {
    $PIPE  = "|   "
    $SPACE = "    "
    $TEE   = "+-- "
    $LAST  = "\-- "
  } else {
    $PIPE  = "│   "
    $SPACE = "    "
    $TEE   = "├── "
    $LAST  = "└── "
  }

  $items = Get-ChildItem -LiteralPath $BasePath |
    Where-Object { $Exclude -notcontains $_.Name } |
    Sort-Object @{Expression={$_.PSIsContainer};Descending=$true}, Name

  $lines = New-Object System.Collections.Generic.List[string]

  for ($i = 0; $i -lt $items.Count; $i++) {
    $item = $items[$i]
    $isLast = ($i -eq $items.Count - 1)

    if ($isLast) { $connector = $LAST } else { $connector = $TEE }
    $lines.Add(("{0}{1}{2}" -f $Prefix, $connector, $item.Name))

    if ($item.PSIsContainer) {
      if ($isLast) { $newPrefix = $Prefix + $SPACE } else { $newPrefix = $Prefix + $PIPE }
      foreach ($line in (Get-TreeLines -BasePath $item.FullName -Prefix $newPrefix)) {
        $lines.Add($line)
      }
    }
  }

  return $lines
}

if (!(Test-Path -LiteralPath $Readme)) {
  throw "README not found: $Readme"
}

$start = "<!-- STRUCTURE:START -->"
$end   = "<!-- STRUCTURE:END -->"

$readmeText = Get-Content -LiteralPath $Readme -Raw

$treeLines = New-Object System.Collections.Generic.List[string]
$treeLines.Add(".")
foreach ($line in (Get-TreeLines -BasePath (Resolve-Path $Path).Path -Prefix "")) {
  $treeLines.Add($line)
}

$treeBlock = @"
$start
``````text
$([string]::Join("`n", $treeLines))
``````
$end
"@

$pattern = [regex]::Escape($start) + ".*?" + [regex]::Escape($end)

if ($readmeText -notmatch $pattern) {
  $regex = New-Object System.Text.RegularExpressions.Regex($pattern, [System.Text.RegularExpressions.RegexOptions]::Singleline)
  if (-not $regex.IsMatch($readmeText)) {
    throw "Markers not found in $Readme. Add:`n$start`n...`n$end"
  }
}

$newReadme = [regex]::Replace(
$readmeText,
$pattern,
[System.Text.RegularExpressions.MatchEvaluator]{ param($m) $treeBlock },
[System.Text.RegularExpressions.RegexOptions]::Singleline
)

$newReadme | Out-File -LiteralPath $Readme -Encoding utf8
Write-Host "Updated $Readme"