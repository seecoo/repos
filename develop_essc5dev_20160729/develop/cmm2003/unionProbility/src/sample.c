	PUnionProbilityGrp 	pprobGrp = NULL;	// ��ÿ�����̵�Ч�ʽ��и���ͳ��

	// ����һ�����ݽṹ
	if ((pprobGrp = UnionNewProbilityGrp()) == NULL)
	{
		printf("UnionNewProbilityGrp failure!\n");
		goto exitNow;
	}
	
	for (;;)
	{
		// �����һ���������С�
		xx()
		for (;;)	// ��ÿ�����룬��һ���������
			UnionAddToProbilityGrp(pprobGrp,paveCount[index]->transAveEfficent);
	}
	UnionGenerateProbilityReport(pprobGrp,outFp);
	UnionPercentAllRecOfProbilityGrp(pprobGrp);
	UnionPrintProbilityGrpToFile(pprobGrp,outFp);
	UnionDeleteProbilityGrp(pprobGrp);

