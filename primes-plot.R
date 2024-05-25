library(data.table)
library(ggplot2)


# 读取质数文件
read_primes <- function(file) {
  primes_data <- fread(file)
  primes <- primes_data$V1
  return(primes)
}

# 分析质数密度
analyze_prime_density <- function(primes, interval_length = 1e6) {
  max_prime <- max(primes)
  num_intervals <- ceiling(max_prime / interval_length)
  
  # 计算每个区间的质数数量
  intervals <- cut(primes, breaks = seq(0, max_prime, by = interval_length), right = FALSE)
  prime_counts <- table(intervals)
  
  # 计算每个区间的质数密度
  densities <- prime_counts / interval_length
  
  # 创建数据框用于绘图
  density_df <- data.frame(
    Interval = seq(0, max_prime - interval_length, by = interval_length),
    Density = as.numeric(densities)
  )
  
  return(density_df)
}

# 读取质数
primes <- read_primes("primes.txt")

# 分析质数密度
density_df <- analyze_prime_density(primes)

# 使用ggplot2绘制质数密度分布的柱状图
plot_density <- function(density_df) {
  ggplot(density_df, aes(x = Interval, y = Density)) +
    geom_bar(stat = "identity", fill = "blue", alpha = 0.7) +
    labs(
      title = "Prime Density Distribution",
      x = "Number Range",
      y = "Density"
    ) +
    theme_minimal() +
    scale_x_continuous(labels = scales::comma)
}

# 绘制图形
plot_density(density_df)

