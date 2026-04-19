# mpi_speedup_test.ps1
param(
    [string]$ExePath = "./a.exe",
    [string]$MatrixFile = "matrix.txt",
    [string]$VectorFile = "vector.txt",
    [int[]]$ProcessList = @(1, 2, 4, 8, 16),
    [int]$RunsPerConfig = 3
)

Write-Host @"
========================================
         MPI Strong Scaling Test
         Matrix Size: 1,000,000 x 1,000,000
         (Pure MPI, no OpenMP)
========================================
Program: $ExePath
Matrix File: $MatrixFile
Vector File: $VectorFile
Processes to test: $ProcessList
Runs per config: $RunsPerConfig
========================================
"@ -ForegroundColor Cyan

$results = @()

foreach ($procs in $ProcessList) {
    Write-Host "`nTesting with $procs MPI processes..." -ForegroundColor Yellow
    
    $times = @()
    
    for ($run = 1; $run -le $RunsPerConfig; $run++) {
        Write-Host "  Run $run / $RunsPerConfig ..." -ForegroundColor Gray
        
        # 纯MPI，不需要设置OMP_NUM_THREADS
        $measure = Measure-Command {
            mpiexec -n $procs $ExePath $MatrixFile $VectorFile 2>$null
        }
        
        $time_sec = $measure.TotalSeconds
        $times += $time_sec
        Write-Host "    Time: $([math]::Round($time_sec, 3)) seconds"
    }
    
    $avg_time = ($times | Measure-Object -Average).Average
    $min_time = ($times | Measure-Object -Minimum).Minimum
    $max_time = ($times | Measure-Object -Maximum).Maximum
    
    $results += [PSCustomObject]@{
        Processes = $procs
        AvgTime = $avg_time
        MinTime = $min_time
        MaxTime = $max_time
        Speedup = $null
        Efficiency = $null
    }
    
    Write-Host "  Average time: $([math]::Round($avg_time, 3)) seconds" -ForegroundColor Green
}

# 计算加速比（以1进程为基准）
$baseline_time = $results[0].AvgTime

foreach ($result in $results) {
    $result.Speedup = [math]::Round($baseline_time / $result.AvgTime, 3)
    $result.Efficiency = [math]::Round($result.Speedup / $result.Processes, 3)
}

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "          Speedup Test Results" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ("{0,-10} {1,-12} {2,-10} {3,-12} {4,-10}" -f "Processes", "Avg Time(s)", "Speedup", "Efficiency", "Eff(%)")
Write-Host ("{0,-10} {1,-12} {2,-10} {3,-12} {4,-10}" -f "---------", "------------", "-------", "----------", "------")

foreach ($result in $results) {
    Write-Host ("{0,-10} {1,-12} {2,-10} {3,-12} {4,-10}" -f 
        $result.Processes, 
        [math]::Round($result.AvgTime, 3),
        $result.Speedup,
        $result.Efficiency,
        "$([math]::Round($result.Efficiency * 100, 1))%")
}

# 性能分析
Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "          Performance Analysis" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

for ($i = 1; $i -lt $results.Count; $i++) {
    $ideal = $results[$i].Processes
    $actual = $results[$i].Speedup
    $efficiency = $results[$i].Efficiency
    
    if ($actual -ge $ideal * 0.8) {
        $grade = "Excellent"
    } elseif ($actual -ge $ideal * 0.6) {
        $grade = "Good"
    } elseif ($actual -ge $ideal * 0.4) {
        $grade = "Fair"
    } else {
        $grade = "Poor - Communication bottleneck"
    }
    
    Write-Host "$($results[$i].Processes) processes: Speedup=$actual (ideal=$ideal), Efficiency=$([math]::Round($efficiency*100,1))% -> $grade"
}

# 保存结果
$timestamp = Get-Date -Format 'yyyyMMdd_HHmmss'
$csv_file = "mpi_speedup_${timestamp}.csv"
$results | Export-Csv -Path $csv_file -NoTypeInformation
Write-Host "`nResults saved to: $csv_file" -ForegroundColor Green