## Set up scales
yscale <- scale_y_log10(limits = c(0.001, 3600), 
                        breaks = trans_breaks("log10", function(x) 10^x, 6), 
                        labels = trans_format("log10", math_format(10^.x)), 
                        oob = scales::rescale_none)
ylabel <- ylab("Runtime (s., log. scale)")
yticks <- annotation_logticks(side="lr")

label_rotator <- theme(axis.text.x = element_text(angle = 30, hjust = 1))

## Replace Inf values in a dataset with huge finite values for charting
blowup_inf <- function(data){
    data[ data == Inf ] <- (3600 * 10^4)
    return(data)
}

## Build a plot of full run data
plot_full <- function(data){
    ## Lay out x scale
    xscale_full <- scale_x_discrete()

    ## Clean up infinite values
    data <- blowup_inf(data)

    ## Melt data
    mdf <- melt(data, id.vars='Input', variable.name = "Algorithm", value.name = "Runtime")

    ## Construct plot
    dataplot <- ggplot(data=mdf) + geom_line(aes(color = Algorithm, x = Input, y = Runtime, group = Algorithm)) + xscale_full + yscale + yticks + ylabel + label_rotator

    return(dataplot)
}

## Build a plot of cutoff data
plot_cutoff <- function(data){
    ## Lay out x scale
    xscale_cutoff <- scale_x_discrete(limits = c(5, 7, 10, 'None'))

    ## Clean up infinite values
    data <- blowup_inf(data)
    
    ## Melt data
    mdf <- melt(data, id.vars='Cutoff', variable.name = "Algorithm", value.name = "Runtime")

    ## Construct plot
    dataplot <- ggplot(data=mdf) + geom_line(aes(color = Algorithm, x = Cutoff, y = Runtime, group = Algorithm)) + xscale_cutoff + yscale + yticks + ylabel

    return(dataplot)
}

## Build a plot of threaded data
plot_threaded <- function(data){
    ## Lay out x scale
    xscale_threads <- scale_x_discrete(limits = c(1, 2, 4, 6, 8, 12, 16))

    ## Clean up infinite values
    data <- blowup_inf(data)
    
    ## Melt data
    mdf <- melt(data, id.vars='Threads', variable.name = "Algorithm", value.name = "Runtime")

    ## Construct plot
    dataplot <- ggplot(data=mdf) + geom_line(aes(color = Algorithm, x = Threads, y = Runtime, group = Algorithm)) + xscale_threads + yscale + yticks + ylabel

    return(dataplot)
}
