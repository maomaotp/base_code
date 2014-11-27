
void show_each_packet(const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
	unsigned int i = 0;
	printf("==================================================================\n");
	for (i = 0; i < pkthdr->len; i++)
	{
		printf("%02x ", packet[i]);
		if (((i % 16 == 15) && (i != 0)))
			printf("\n");
		if (i == 143)
			printf("\n");
	}
	printf("\n==================================================================\n");
}
