# thread_speedup_test.ps1
param(
    [string]$ExePath = "./solver.exe",
    [string]$MatrixFile = "matrix.txt",
    [string]$VectorFile = "vector.txt",
    [int[]]$ThreadList = @(1, 2, 4, 8, 16),
    [int]$RunsPerConfig = 3
)

Write-Host @"
========================================
   MPI + OpenMP Sparse Solver Speedup Test
   Matrix Size: 1,000,000 x 1,000,000
   (1 MPI process, varying threads)
========================================
Program: $ExePath
Matrix File: $MatrixFile
Vector File: $VectorFile
Threads to test: $ThreadList
Runs per config: $RunsPerConfig
========================================
"@ -ForegroundColor Cyan

$results = @()

foreach ($threads in $ThreadList) {
    Write-Host "`nTesting with $threads threads..." -ForegroundColor Yellow
    
    $times = @()
    
    for ($run = 1; $run -le $RunsPerConfig; $run++) {
        Write-Host "  Run $run / $RunsPerConfig ..." -ForegroundColor Gray
        
        # 设置线程数
        $env:OMP_NUM_THREADS = $threads
        
        $measure = Measure-Command {
            mpiexec -n 1 $ExePath $MatrixFile $VectorFile 2>$null
        }
        
        $time_sec = $measure.TotalSeconds
        $times += $time_sec
        Write-Host "    Time: $([math]::Round($time_sec, 3)) seconds"
    }
    
    $avg_time = ($times | Measure-Object -Average).Average
    $min_time = ($times | Measure-Object -Minimum).Minimum
    $max_time = ($times | Measure-Object -Maximum).Maximum
    
    $results += [PSCustomObject]@{
        Threads = $threads
        AvgTime = $avg_time
        MinTime = $min_time
        MaxTime = $max_time
        Speedup = $null
        Efficiency = $null
    }
    
    Write-Host "  Average time: $([math]::Round($avg_time, 3)) seconds" -ForegroundColor Green
}

# 计算加速比（以1线程为基准）
$baseline_time = ($results | Where-Object { $_.Threads -eq $ThreadList[0] }).AvgTime

foreach ($result in $results) {
    $result.Speedup = [math]::Round($baseline_time / $result.AvgTime, 3)
    $result.Efficiency = [math]::Round($result.Speedup / $result.Threads, 3)
}

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "       Thread Speedup Test Results" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ("{0,-10} {1,-12} {2,-10} {3,-12} {4,-10}" -f "Threads", "Avg Time(s)", "Speedup", "Efficiency", "Eff(%)")
Write-Host ("{0,-10} {1,-12} {2,-10} {3,-12} {4,-10}" -f "-------", "------------", "-------", "----------", "------")

foreach ($result in $results) {
    Write-Host ("{0,-10} {1,-12} {2,-10} {3,-12} {4,-10}" -f 
        $result.Threads, 
        [math]::Round($result.AvgTime, 3),
        $result.Speedup,
        $result.Efficiency,
        "$([math]::Round($result.Efficiency * 100, 1))%")
}

# 分析并行效率
Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "            Performance Analysis" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

$best_speedup = ($results | Measure-Object -Maximum Speedup).Maximum
$best_threads = ($results | Where-Object { $_.Speedup -eq $best_speedup }).Threads

Write-Host "Best speedup: $best_speedup at $best_threads threads"
Write-Host ""

# 检查是否达到线性加速
for ($i = 1; $i -lt $results.Count; $i++) {
    $ideal = $results[$i].Threads
    $actual = $results[$i].Speedup
    $efficiency = $results[$i].Efficiency
    
    if ($actual -ge $ideal * 0.9) {
        $grade = "Excellent"
    } elseif ($actual -ge $ideal * 0.7) {
        $grade = "Good"
    } elseif ($actual -ge $ideal * 0.5) {
        $grade = "Fair"
    } else {
        $grade = "Poor"
    }
    
    Write-Host "$($results[$i].Threads) threads: Speedup=$actual (ideal=$ideal), Efficiency=$([math]::Round($efficiency*100,1))% -> $grade"
}

$csv_file = "thread_speedup_results_$(Get-Date -Format 'yyyyMMdd_HHmmss').csv"
$results | Export-Csv -Path $csv_file -NoTypeInformation
Write-Host "`nResults saved to: $csv_file" -ForegroundColor Green