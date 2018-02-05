	PUnionProbilityGrp 	pprobGrp = NULL;	// 对每个进程的效率进行概率统计

	// 分配一个数据结构
	if ((pprobGrp = UnionNewProbilityGrp()) == NULL)
	{
		printf("UnionNewProbilityGrp failure!\n");
		goto exitNow;
	}
	
	for (;;)
	{
		// 计算出一个号码序列。
		xx()
		for (;;)	// 将每个号码，调一个这个函数
			UnionAddToProbilityGrp(pprobGrp,paveCount[index]->transAveEfficent);
	}
	UnionGenerateProbilityReport(pprobGrp,outFp);
	UnionPercentAllRecOfProbilityGrp(pprobGrp);
	UnionPrintProbilityGrpToFile(pprobGrp,outFp);
	UnionDeleteProbilityGrp(pprobGrp);

