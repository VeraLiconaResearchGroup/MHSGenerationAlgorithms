# Set up scales
yscale = scale_y_log10(limits = c(0.001, 3600), breaks=c(0.001, 0.01, 0.1, 1, 10, 100, 1000))
ylabel = ylab("Runtime (s., log. scale)")
yticks = annotation_logticks(side="lr")

# Build a plot of full run data
plot_full <- function(data){
    # Lay out x scale
    xscale_full = scale_x_discrete()

    # Format labels on x axis
    x_label_rotator = theme(axis.text.x = element_text(angle = 30, hjust = 1))

    # Melt data
    mdf <- melt(data, id.vars='Input', variable.name = "Algorithm", value.name = "Runtime")

    # Construct plot
    dataplot = ggplot(data=mdf) + geom_line(aes(linetype = Algorithm, x = Input, y = Runtime, group = Algorithm)) + xscale_full +  x_label_rotator + yscale + yticks + ylabel

    return(dataplot)
}

# Build a plot of cutoff data
plot_cutoff <- function(data){
    # Lay out x scale
    xscale_cutoff = scale_x_discrete(limits = c(5, 7, 10, 'None'))

    # Melt data
    mdf <- melt(data, id.vars='Cutoff', variable.name = "Algorithm", value.name = "Runtime")

    # Construct plot
    dataplot = ggplot(data=mdf) + geom_line(aes(linetype = Algorithm, x = Cutoff, y = Runtime, group = Algorithm)) + xscale_cutoff + yscale + yticks + ylabel

    return(dataplot)
}

# Build a plot of threaded data
plot_threaded <- function(data){
    # Lay out x scale
    xscale_threads = scale_x_discrete(limits = c(1, 2, 4, 8, 12, 16))

    # Melt data
    mdf <- melt(data, id.vars='Threads', variable.name = "Algorithm", value.name = "Runtime")

    # Construct plot
    dataplot = ggplot(data=mdf) + geom_line(aes(linetype = Algorithm, x = Threads, y = Runtime, group = Algorithm)) + xscale_threads + yscale + yticks + ylabel

    return(dataplot)
}
